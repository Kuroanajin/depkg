// adapted from InstallDragDropPkg in main_window.cpp

#include <algorithm>
#include <atomic>
#include <exception>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include "core/file_format/pkg.h"
#include "core/file_format/psf.h"
#include "common/path_util.h"
#include "common/string_util.h"
#include "core/loader.h"

static bool ExtractAllFiles(const PKG& pkg, int nfiles){
	std::atomic<int> next_index = 0;
	std::atomic<bool> failed = false;
	std::mutex print_mutex;
	int completed = 0; // only changes under print_mutex, so the printed count always goes up

	const auto worker = [&](){
		while(!failed){
			const int i = next_index++;
			if(i >= nfiles){
				break;
			}

			try{
				if(pkg.ExtractFiles(i)){
					std::lock_guard lock(print_mutex);
					completed++;
					std::cout << "\r      Extracting file " << completed << " of " << nfiles << std::flush;
				}else{
					failed = true;
				}
			}catch(const std::exception& e){
				failed = true;
				std::lock_guard lock(print_mutex);
				std::cout << "\nCannot extract file : " << e.what() << std::endl;
			}
		}
	};

	std::vector<std::thread> threads(std::max(1u, std::thread::hardware_concurrency()));
	for(auto& thread : threads){
		thread = std::thread(worker);
	}
	for(auto& thread : threads){
		thread.join();
	}

	return !failed;
}

int main(int argc, char** argv){
	std::filesystem::path file = "";
	std::filesystem::path output_folder_path = "";
	int exit_code = 1; // only a complete extraction clears it

	if(argc > 1){
		file += argv[1];
		
		if(argc > 2){
			output_folder_path += argv[2];
		}
	}
	
	if (Loader::DetectFileType(file) == Loader::FileTypes::Pkg) {
		std::cout << file << " is a valid PKG\n" << std::endl;
		
		PKG pkg = PKG();
		
		std::string failreason;
		if (!pkg.Open(file, failreason)) {
            std::cout << "Cannot open PKG file : " << failreason << std::endl;
        }else{
		//	std::cout << "open PKG file success" << std::endl;
			
			PSF psf = PSF();
			
			if (!psf.Open(pkg.sfo)) {
				std::cout << "Could not read SFO." << std::endl;
			}else{
			//	std::cout << "open PSF success" << std::endl;
				
				output_folder_path /= pkg.GetTitleID();
				
				std::string category;
				category += *psf.GetString("CATEGORY");
			//	std::cout << "PSF category = " << category << std::endl;
				
				std::string pkgType = pkg.GetPkgFlags();
			//	std::cout << "pkgType = " << pkgType << std::endl;
				
				int ret = 0;
				
				if(pkgType.contains("PATCH")){
					std::cout << "pkg is a game update" << std::endl;
					ret = 102;
					output_folder_path += "-patch";
				}else if(category == "ac"){
					std::cout << "pkg is a dlc" << std::endl;
					ret = 103;
					std::string content_id;
					if (auto value = psf.GetString("CONTENT_ID"); value.has_value()) {
						content_id = std::string{*value};
						std::string entitlement_label = Common::SplitString(content_id, '-')[2];
						output_folder_path /= entitlement_label;
					} else {
						std::cout << "PSF file there is no CONTENT_ID" << std::endl;
					}
				}else{
					std::cout << "pkg is a base game" << std::endl;
					ret = 101;
				}
				
				if(argc == 3 && std::string("--check-type") == argv[2])
				{
					return ret;
				}
				
				std::cout << "Extracting pkg to " << output_folder_path << std::endl;
				
				if (!pkg.Extract(file, output_folder_path, failreason)) {
					std::cout << "Cannot extract PKG file : " << failreason << std::endl;
				} else if(ExtractAllFiles(pkg, pkg.GetNumberOfFiles())) {
					exit_code = 0;
				}
			}
		}
	} else {
		std::cout << file << " doesn't appear to be a valid PKG file" << std::endl;
	}
	
	std::cout << "\nTHE END " << file << std::endl;
	
	if(argc == 2){
		std::cout << "\nPress [enter] to exit." << std::endl;
		std::cin.get();
	}

	return exit_code;
}
