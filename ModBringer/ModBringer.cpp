// ModBringer.cpp 
#
#include <iostream>
#include <iterator>
#include <filesystem>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <chrono> // Used for identifying file timestamps
#include <stdio.h> // Used for identifying OS 
#include <limits>  // Used by the Pause function

// MSVC complains about std::getenv right now we are disabling the warning
// std::getenv is only used when the OS is Macintosh
#pragma warning(disable : 4996) 

enum class Options {
    Setup,
    Modifications,
    Maintenance,
    Help,
    Exit
};

struct Settings {
    std::filesystem::path gameDirectory;
    std::filesystem::path contentDirectory;
    std::string operatingSystem{};
};

bool verifyDirectory(std::filesystem::path directory);

void restoreChanged(std::filesystem::path activeDirectory, std::filesystem::path backupDirectory, std::string extension);
void restoreAll(std::filesystem::path contentDirectory);
int getSelection();
void handleModSelection(std::filesystem::path contentPath, std::filesystem::path activeFilePath, std::filesystem::path backupDirectory, std::string fileType, std::map<int, std::string> map, int userSelection);
void pause();

int main()
{
    std::cout <<
        " _______  _______  ______   ______   _______ _________ _        _______  _______  _______ \n"
        "(       )(  ___  )(  __  \\ (  ___ \\ (  ____ )\\__   __/( (    /|(  ____ \\(  ____ \\(  ____ )\n"
        "| () () || (   ) || (  \\  )| (   ) )| (    )|   ) (   |  \\  ( || (    \\/| (    \\/| (    )|\n"
        "| || || || |   | || |   ) || (__/ / | (____)|   | |   |   \\ | || |      | (__    | (____)|\n"
        "| |(_)| || |   | || |   | ||  __ (  |     __)   | |   | (\\ \\) || | ____ |  __)   |     __)\n"
        "| |   | || |   | || |   ) || (  \\ \\ | (\\ (      | |   | | \\   || | \\_  )| (      | (\\ (   \n"
        "| )   ( || (___) || (__/  )| )___) )| ) \\ \\_____) (___| )  \\  || (___) || (____/\\| ) \\ \\__\n"
        "|/     \\|(_______)(______/ |/ \\___/ |/   \\__/\\_______/|/    )_)(_______)(_______/|/   \\__/\n"
        "by Squiblydoo\n" << std::endl;

    // Detect Operating System
    Settings settings;
#ifdef _WIN64
    settings.operatingSystem = "Windows";
#elif __APPLE__ || __MACH__
    settings.operatingSystem = "Macintosh";
#elif __linux__
    settings.operatingSystem = "Linux";
#elif __unix__
    settings.operatingSystem = "Linux";
#endif

    int selection;

     // Check for existing configuration file; if it exists, read in the current settings
    const auto modBringer_path = std::filesystem::current_path();

    if (exists(modBringer_path / "ModBringer.config")) {
        std::ifstream config{ "ModBringer.config" };
        std::cout << "Current Settings: \n";
        std::string savedGameDirectory;
        std::getline(config, savedGameDirectory);
        savedGameDirectory.erase(remove(savedGameDirectory.begin(), savedGameDirectory.end(), '\"'), savedGameDirectory.end());
        settings.gameDirectory = savedGameDirectory;
        settings.contentDirectory = settings.gameDirectory / "Content";
        std::cout << "Game Directory: " << savedGameDirectory << "\n";
        std::cout << "Operating System: " << settings.operatingSystem;
        std::cout << std::endl;
    }
    

    while (true) {
        std::cout << "\nType number to section an option.\nOptions\n"
            "[0] Setup or Update\n"
            "[1] Select Modifications \n"
            "[2] ModBringer Maintenance\n"
            "[3] Help\n" 
            "[4] Exit\n" << std::endl;

        std::cin >> selection;
        Options userSelected = static_cast<Options>(selection);


        switch (userSelected) {
        case Options::Exit: {
            std::cout << "Option " << selection << " selected\nExiting." << std::endl;
            std::exit(0);

        } case Options::Setup: {
            std::cout << "Option " << selection << " selected\n";
            if (!verifyDirectory(settings.gameDirectory)) { // Game directory not set

                //Check common places first; if they fail, ask the user.
                if (settings.operatingSystem == "Windows") {

                    auto defaultWindowsPath = std::filesystem::path{ R"(C:\Program Files (x86)\Steam\steamapps\common\ScourgeBringer\)" };
                    if (verifyDirectory(defaultWindowsPath)) {
                        std::filesystem::path contentFolder = defaultWindowsPath / "Content";
                        contentFolder.make_preferred();
                        std::ofstream config{ "ModBringer.config", std::ios::out | std::ios::in | std::ios::trunc };
                        config << defaultWindowsPath << std::endl;
                        settings.gameDirectory = defaultWindowsPath;
                        settings.contentDirectory = contentFolder;
                        settings.operatingSystem = "Windows";
                    }   
                    else {
                        std::string userProvidedGameDirectory;
                        std::filesystem::path userProvidedPath;

                        do {
                            std::cout << "Game files could not be found.\nIf the game is installed, please specify the game directory:\nExample:\nC:\\Program Files (x86)\\Steam\\steamapps\\common\\ScourgeBringer\n";
                            std::cin.ignore(100, '\n');
                            getline(std::cin, userProvidedGameDirectory);
                            userProvidedPath = userProvidedGameDirectory;
                        } while (!verifyDirectory(userProvidedPath));
                        std::filesystem::path contentFolder = userProvidedPath;
                        std::ofstream config{ "ModBringer.config", std::ios::out | std::ios::in | std::ios::trunc };
                        config << contentFolder << std::endl;
                        settings.gameDirectory = userProvidedPath;
                        settings.contentDirectory = contentFolder;
                        std::cout << "Saving game Directory\n";
                    };
                }
                else if (settings.operatingSystem == "Macintosh") {
                    
                    auto home = std::getenv("HOME");
                    auto directory = std::filesystem::path{ home };
                    std::filesystem::path defaultMacPath = directory / "Library/Application Support/Steam/steamapps/common/ScourgeBringer/ScourgeBringer.app/Contents/Resources/";
                
                    if (verifyDirectory(defaultMacPath)) {
                        std::filesystem::path contentFolder = defaultMacPath / "Content";
                        contentFolder.make_preferred();
                        std::ofstream config{ "ModBringer.config", std::ios::out | std::ios::in | std::ios::trunc };
                        config << defaultMacPath << std::endl;
                        settings.gameDirectory = defaultMacPath;
                        settings.contentDirectory = contentFolder;
                        settings.operatingSystem = "Macintosh";

                    }
                
                }
                else if (settings.operatingSystem == "Linux") {
                    auto home = std::getenv("HOME");
                    auto directory = std::filesystem::path{ home };
                    std::filesystem::path defaultLinuxPath = directory / ".steam/steam/steamapps/common/ScourgeBringer/";

                    if (verifyDirectory(defaultLinuxPath)) {
                        std::filesystem::path contentFolder = defaultLinuxPath / "Content";
                        contentFolder.make_preferred();
                        std::ofstream config{ "ModBringer.config", std::ios::out | std::ios::in | std::ios::trunc };
                        config << defaultLinuxPath << std::endl;
                        settings.gameDirectory = defaultLinuxPath;
                        settings.contentDirectory = contentFolder;
                        settings.operatingSystem = "Linux";
                    }   
                    else {
                        std::string userProvidedGameDirectory;
                        std::filesystem::path userProvidedPath;

                        do {
                            std::cout << "Game files could not be found.\nIf the game is installed, please specify the game directory:\nExample:\n.steam/steam/steamapps/common/ScourgeBringer/";
                            std::cin.ignore(100, '\n');
                            getline(std::cin, userProvidedGameDirectory);
                            userProvidedPath = userProvidedGameDirectory;
                        } while (!verifyDirectory(userProvidedPath));
                        std::filesystem::path contentFolder = userProvidedPath;
                        std::ofstream config{ "ModBringer.config", std::ios::out | std::ios::in | std::ios::trunc };
                        config << contentFolder << std::endl;
                        settings.gameDirectory = userProvidedPath;
                        settings.contentDirectory = contentFolder;
                        std::cout << "Saving game Directory\n";
                    };

                }
               
             

            }
            const auto contentFolder = settings.contentDirectory;
            const auto modFolder = contentFolder / "Mods";
            const auto modFiles = modBringer_path / "Mods";


            // If the mod directory exists, we just need to update it with the items in the directory next to ModBringer.
            const auto copyOptions = std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive;
            try {
                std::filesystem::copy(modFiles, modFolder, copyOptions);
            }
            catch (std::exception &e) {
                std::cout << "Perhaps the mod folder doesn't exist or you don't have the proper permissions to write here : " << e.what();
            }


            // Setup for backing up original gamefiles
            const auto backupSkinDirectory = modFolder / "Skins/DefaultSkin";
            const auto generalBackup = modFolder / "Backups";
            const auto gameplayMods = modFolder / "Gameplay Mods";
            const auto defaultGameplay = gameplayMods / "Default Gameplay";
            const auto tilesetDirectory = modFolder / "Tileset Mods";
            const auto defaultTilesets = tilesetDirectory / "Default Tilesets";
            const auto bossMods = modFolder / "Boss Mod Images";
            const auto defaultBosses = bossMods / "Default Bossess";
            const auto languageFileMods = modFolder / "Language File Mods";
            const auto defaultLanguageFiles = languageFileMods / "Default Language Files";
            const auto miscImageMods = modFolder / "Misc Images Mods";
            const auto defaultMiscImages = miscImageMods / "Default Misc Images";
            const auto backgroundMods = modFolder / "Background Mods";
            const auto defaultBackgrounds = backgroundMods / "Default Backgrounds";
            const auto modPackFolder = modFolder / "Mod Packs";
            const auto moddedFonts = modFolder / "Modded Fonts";
            const auto defaultFonts = moddedFonts / "Default Fonts";


            // Creates BackupSkin Directory:
                 // Creates Backups of "common_X.xnb" files
            if (!verifyDirectory(backupSkinDirectory)) {
                std::filesystem::create_directory(backupSkinDirectory);
                for (auto entry : std::filesystem::directory_iterator(contentFolder)) {
                    if (entry.is_regular_file() && entry.path().has_extension()) {
                        std::filesystem::copy(entry, backupSkinDirectory);
                    };
                };
            }

            // Backup Catalog files
            if (!exists(defaultGameplay)) {
                std::filesystem::create_directory(gameplayMods);
                std::filesystem::create_directory(defaultGameplay);

                //  Catalog files don't have an extension but are a regular file
                for (auto entry : std::filesystem::directory_iterator(contentFolder)) {
                    if (entry.is_regular_file() && !entry.path().has_extension()) {
                        std::filesystem::copy(entry, defaultGameplay);
                    }
                    if (entry.path().extension() == ".ini") {
                        std::filesystem::copy(entry, defaultGameplay);
                    }
                };
                std::cout << "Backups of Gameplay files created." << std::endl;
            }

            // Backup Tilesets
            if (!exists(defaultTilesets)) {
                std::filesystem::create_directory(tilesetDirectory);
                std::filesystem::create_directory(defaultTilesets);
                for (auto entry : std::filesystem::directory_iterator(contentFolder / "Tilesets")) {
                    std::filesystem::copy(entry, defaultTilesets);
                }
                std::cout << "Backups of Tilesets created." << std::endl;
            }

            // Backup Boss Image Files
            if (!exists(defaultBosses)) {
                std::filesystem::create_directory(bossMods);
                std::filesystem::create_directory(defaultBosses);
                for (auto entry : std::filesystem::directory_iterator(contentFolder / "Bosses")) {
                    std::filesystem::copy(entry, defaultBosses);
                }
                std::cout << "Backups of Boss sprites created." << std::endl;
            }

            // Backup Localization files
            if (!exists(defaultLanguageFiles)) {
                std::filesystem::create_directory(languageFileMods);
                std::filesystem::create_directory(defaultLanguageFiles);
                for (auto entry : std::filesystem::directory_iterator(contentFolder / "Localizations")) {
                    std::filesystem::copy(entry, defaultLanguageFiles);
                }
                std::cout << "Backups of Localization files created." << std::endl;
            }

            // Backup Misc Files
            if (!exists(defaultMiscImages)) {
                std::filesystem::create_directory(miscImageMods);
                std::filesystem::create_directory(defaultMiscImages);
                for (auto entry : std::filesystem::directory_iterator(contentFolder / "Doors")) {
                    std::filesystem::copy(entry, defaultMiscImages);
                }
                std::cout << "Backups of Misc Sprites created." << std::endl;
            }

            // Backup background images
            if (!exists(defaultBackgrounds)) {
                std::filesystem::create_directory(backgroundMods);
                std::filesystem::create_directory(defaultBackgrounds);
                for (auto entry : std::filesystem::directory_iterator(contentFolder / "Backgrounds")) {
                    std::filesystem::copy(entry, defaultBackgrounds);
                }
                std::cout << "Backups of Background Sprites created." << std::endl;
            }

            // Backup font files
            if (!exists(defaultFonts)) {
                std::filesystem::create_directory(moddedFonts);
                std::filesystem::create_directory(defaultFonts);
                for (auto entry : std::filesystem::directory_iterator(contentFolder / "Fonts")) {
                    std::filesystem::copy(entry, defaultFonts);
                }
                std::cout << "Backups of Background Sprites created." << std::endl;
            }


            if (!exists(modPackFolder)) {
                std::filesystem::create_directory(modPackFolder);
            }


            // Make backup of player save
            // TODO: Rename save in order to make a timestamped backup each time setup is ran
            if (!exists(generalBackup)) std::filesystem::create_directory(generalBackup);
            const auto backupCopyOptions = std::filesystem::copy_options::update_existing;

            std::filesystem::path modBringerDirectory = std::filesystem::current_path();

            // Both of these account for the save being in a User directory and copy the file by changing the working directory to that directory.
            if (settings.operatingSystem == "Windows") {
                std::filesystem::path tempDirectoryForSave = std::filesystem::temp_directory_path();
                std::filesystem::current_path(tempDirectoryForSave);
                std::filesystem::current_path(R"(../../LocalLow/Flying Oak Games/ScourgeBringer)");
                std::filesystem::copy(std::filesystem::current_path() / "0.sav", generalBackup, backupCopyOptions);

                //Rename save with timestamp.
                std::stringstream saveTimeStamp{};
                const time_t timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                saveTimeStamp << std::put_time(std::localtime(&timenow), "%F-%H%M");
                std::string timeStampString = "0.sav" + saveTimeStamp.str();
                std::filesystem::rename(generalBackup / "0.sav", generalBackup / timeStampString);

                std::cout << "Backup of ScourgeBringer save file created as " << timeStampString << std::endl;
            }
            else if (settings.operatingSystem == "Macintosh") {
                auto home = getenv("HOME");
                auto directory = std::filesystem::path{ home };
                std::filesystem::path tempDirectoryForSave = directory / "Library/Application Support/Flying Oak Games/ScourgeBringer";
                std::filesystem::current_path(tempDirectoryForSave);
                std::filesystem::copy(std::filesystem::current_path() / "0.sav", generalBackup, backupCopyOptions);
               
                //Rename save with timestamp.
                std::stringstream saveTimeStamp{};
                const time_t timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                saveTimeStamp << std::put_time(std::localtime(&timenow), "%F-%H%M");
                std::string timeStampString = "0.sav" + saveTimeStamp.str();
                std::filesystem::rename(generalBackup / "0.sav", generalBackup / timeStampString);

                std::cout << "Backup of ScourgeBringer save file created as " << timeStampString << std::endl;
            }
                else if (settings.operatingSystem == "Linux") {
                auto home = getenv("HOME");
                auto directory = std::filesystem::path{ home };
                std::filesystem::path tempDirectoryForSave = directory / ".scourgebringer";
                std::filesystem::current_path(tempDirectoryForSave);
                std::filesystem::copy(std::filesystem::current_path() / "0.sav", generalBackup, backupCopyOptions);
               
                //Rename save with timestamp.
                std::stringstream saveTimeStamp{};
                const time_t timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                saveTimeStamp << std::put_time(std::localtime(&timenow), "%F-%H%M");
                std::string timeStampString = "0.sav" + saveTimeStamp.str();
                std::filesystem::rename(generalBackup / "0.sav", generalBackup / timeStampString);

                std::cout << "Backup of ScourgeBringer save file created as " << timeStampString << std::endl;
            }

            // Return working directory to previous directory
            std::filesystem::current_path(modBringerDirectory);

            continue;

        } case Options::Modifications: {
            std::cout << "Option " << selection << " selected\n";
            if (settings.contentDirectory == "" || !verifyDirectory(settings.contentDirectory / "Mods")) {
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                    << "!!!Mod directory does not exist. Please perform setup first.!!!\n"
                    << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                    << std::endl;
                continue;
            };
            if (exists(settings.contentDirectory / "Mods")) {

                // Iterate through the possible Mod Categories
                int entryNum{ 0 };
                std::map<int, std::string> modTypes;
                int typeNum{ 0 };
                for (auto entry : std::filesystem::directory_iterator(settings.contentDirectory / "Mods")) {
                    // Exclude the backup folder from the Mod list. It is in this directory for convienance. 
                    if (entry.path().filename().string() == "Backups") continue;
                    if (entry.path().filename().string() == ".DS_Store") continue; // Exclude this file if it occurs on Macintosh

                    std::cout << "[" << typeNum << "] ";
                    std::cout << entry.path().filename().string() << "\n";
                    modTypes.emplace(typeNum, entry.path().filename().string());
                    typeNum++;
                }
                modTypes.emplace(typeNum, "Cancel");
                std::cout << "[" << typeNum << "] Cancel" << std::endl;
                int modSelection = getSelection();
                
                try {
                    std::cout << modTypes.at(modSelection) << "\n";

                    if (modTypes.at(modSelection) == "Cancel") {
                        std::cout << "Canceled.\nReturning to main menu.\n\n ";
                        continue;
                    }
                    // Handle Gameplay Mods
                    if (modTypes.at(modSelection) == "Gameplay Mods") {
                        handleModSelection(settings.contentDirectory, settings.contentDirectory, settings.contentDirectory / "Mods" / "Gameplay Mods" / "Default Gameplay", "", modTypes, modSelection);
                    };

                    //Handle Tileset Selection
                    if (modTypes.at(modSelection) == "Tileset Mods") {
                        handleModSelection(settings.contentDirectory, settings.contentDirectory / "Tilesets", settings.contentDirectory / "Mods" / "Tileset Mods" / "Default Tilesets", ".xnb", modTypes, modSelection);
                    };

                    // Handle Skin Selection
                    if (modTypes.at(modSelection) == "Skins") {
                        handleModSelection(settings.contentDirectory, settings.contentDirectory, settings.contentDirectory / "Mods" / "Skins" / "DefaultSkin", ".xnb", modTypes, modSelection);
                    }

                    // Handle Background Mods
                    if (modTypes.at(modSelection) == "Background Mods") {
                        handleModSelection(settings.contentDirectory, settings.contentDirectory / "Backgrounds", settings.contentDirectory / "Mods" / "Background Mods" / "Default Backgrounds", ".xnb", modTypes, modSelection);
                    }

                    // Handle Misc Images Mods
                    if (modTypes.at(modSelection) == "Misc Images Mods") {
                        handleModSelection(settings.contentDirectory, settings.contentDirectory / "Doors", settings.contentDirectory / "Mods" / "Misc Images Mods" / "Default Misc Images", ".xnb", modTypes, modSelection);
                    }

                    // Handle Language File Mods
                    if (modTypes.at(modSelection) == "Language File Mods") {
                        handleModSelection(settings.contentDirectory, settings.contentDirectory / "Localizations", settings.contentDirectory / "Mods" / "Language File Mods" / "Default Language Files", "", modTypes, modSelection);
                    }

                    // Handle Boss Mod Images
                    if (modTypes.at(modSelection) == "Boss Mod Images") {
                        handleModSelection(settings.contentDirectory, settings.contentDirectory / "Bosses", settings.contentDirectory / "Mods" / "Boss Mod Images" / "Default Bossess", ".xnb", modTypes, modSelection);
                    }

                    // Handle Mod Packs
                    if (modTypes.at(modSelection) == "Mod Packs") {
                        // TO DO:
                        // Figure out how to change and revert Mod Packs easily
                            //Get Mod Category
                        auto selectedModDirectory = settings.contentDirectory / "Mods" / modTypes.at(modSelection);
                        std::map<int, std::string> modPacks;
                        int modNumber{};
                        std::string restoreDefault = "Restore Default";
                        modPacks.emplace(modNumber, restoreDefault);
                        
                        std::cout << "[" << modNumber << "] ";
                        std::cout << restoreDefault << "\n";
                        modNumber++;
                        for (auto entry : std::filesystem::directory_iterator(selectedModDirectory)) {
                            std::cout << "[" << modNumber << "] ";
                            std::cout << entry.path().filename().string() << "\n";
                            modPacks.emplace(modNumber, entry.path().filename().string());
                            modNumber++;
                        }
                        std::cout << "[" << modNumber << "] Cancel\n";
                        int modPackSelection{};
                        modPackSelection = getSelection();
                        try {
                            std::cout << "You want to replace the current " << modTypes.at(modSelection) << " with " << modPacks.at(modPackSelection) << "?\n";
                            std::cout << "[y/n] ";
                            std::string answer;
                            std::cin >> answer;
                            if (answer == "y") {
                                const auto replaceOptions = std::filesystem::copy_options::overwrite_existing;
                                auto selectedMod = selectedModDirectory / modPacks.at(modPackSelection);
                                restoreAll(settings.contentDirectory);

                                /*
                                If the user wants to restore defaults, we will perform the restoreAll function and do nothing else.
                                Before changing Mod Packs, we will always restore all other changes to the original settings.
                                */
                                if (modPacks.at(modPackSelection) != restoreDefault)
                                {
                                    for (auto& entry : std::filesystem::directory_iterator{ selectedMod }) {
                                        std::string fileStart = entry.path().filename().string().substr(0, 3);
                                        /*
                                        Mod Packs are different mod types, so we will read some of the file name
                                        and then move them to the appropriate directory.
                                        */
                                        // Handle Gameplay files
                                        if (fileStart == "Bos" || 
                                            fileStart == "Bul" ||
                                            fileStart == "Emi" ||
                                            fileStart == "Ene" ||
                                            fileStart == "Map" ||
                                            fileStart == "Par" ||
                                            fileStart == "Roo" ||
                                            fileStart == "Gam"
                                            ) {
                                            std::filesystem::copy(entry, settings.contentDirectory , replaceOptions);
                                            std::cout << "Copying: " << entry.path().filename().string() << "\n";
                                        }
                                        // Handle Skins
                                        if (fileStart == "com") {
                                            std::filesystem::copy(entry, settings.contentDirectory, replaceOptions);
                                            std::cout << "Copying: " << entry.path().filename().string() << "\n";
                                        }
                                        //Handle Tilesets
                                        if (fileStart == "til") {
                                            std::filesystem::copy(entry, settings.contentDirectory / "Tilesets", replaceOptions);
                                            std::cout << "Copying: " << entry.path().filename().string() << "\n";
                                        }
                                        // Handle Misc Images/Doors
                                        if (fileStart == "doo") {
                                            std::filesystem::copy(entry, settings.contentDirectory / "Doors", replaceOptions);
                                            std::cout << "Copying: " << entry.path().filename().string() << "\n";
                                        }
                                        // Handle Boss images
                                        if (fileStart == "bos") {
                                            std::filesystem::copy(entry, settings.contentDirectory / "Bosses", replaceOptions);
                                            std::cout << "Copying: " << entry.path().filename().string() << "\n";
                                        }
                                        // Handle Backgrounds
                                        if (fileStart == "bac") {
                                            std::filesystem::copy(entry, settings.contentDirectory / "Backgrounds", replaceOptions);
                                            std::cout << "Copying: " << entry.path().filename().string() << "\n";

                                        }
                                        // Account for font files
                                        if (fileStart == "pix" ||
                                            fileStart == "hig"
                                            ) {
                                            std::filesystem::copy(entry, settings.contentDirectory / "Fonts", replaceOptions);
                                            std::cout << "Copying: " << entry.path().filename().string() << "\n";
                                        }

                                        // Account for localization mods. The names all differ but are only 2 char in length
                                        if (entry.path().filename().string().length() == 2 || entry.path().filename().string().length() == 3) {
                                            std::filesystem::copy(entry, settings.contentDirectory / "Localizations", replaceOptions);
                                            std::cout << "Copying: " << entry.path().filename().string() << "\n";
                                        }


                                    }
                                };
                            }
                        }
                        catch (std::exception e) {
                            std::cout << "Exception: " << e.what();
                        }
                    }

                }
                catch (std::exception& e) {
                    std::cout << "Exception: " << e.what();
                }
            }
            continue;

        } case Options::Maintenance : {
            std::cout << "Option " << selection << " selected\n";

                if (settings.contentDirectory == "" || !verifyDirectory(settings.contentDirectory / "Mods")) {
                    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                        << "!!!Mod directory does not exist. Please perform setup first.!!!\n"
                        << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                        << std::endl;
                    continue;
                }

                std::cout << "[0] Delete Mod Folder\n"
                    << "[1] Restore Save from backup\n"
                    << "[2] Restore to default game\n"
                    << "[3] Specify different game directory\n"
                    << "[4] Cancel\n"
                    << std::endl;
                int maintenanceSelection = getSelection();

                switch (maintenanceSelection) {
                case (0): {
                    std::cout << "You want to remove the Mod Folder and all mods?\nThis will also restore all game files to their original state before mods.\n";
                    std::cout << "[y/n]: ";
                    std::string answer;
                    std::cin >> answer;
                    if (answer == "y") {
                        restoreAll(settings.contentDirectory);
                        std::cout << "Removing Mod Folder.\n To use Mods again, run Setup from the main menu." << std::endl;
                        std::filesystem::remove_all(settings.contentDirectory / "Mods");
                    }
                    else {
                        std::cout << "Canceled.\nReturning to main menu.\n\n ";
                    }
                    continue;
                }
                case (1): { // Restore Save file
                    std::cout << "Use this only if your save is corrupted and you need to write over your current save.\n"
                        << "This should be unlikely, but could happen if the Gameplay Mods are incompatible with your save.\n"
                        << "Do you want to restore a save?\n";
                    std::cout << "[y/n]: ";
                    std::string answer;
                    std::cin >> answer;
                    if (answer == "y") {
                        std::cout << "Please select a save:\n";

                        std::filesystem::path generalBackup = settings.contentDirectory / "Mods" / "Backups";
                        std::map<int, std::string> backups;
                        int backupNumber{ 0 };
                        for (auto entry : std::filesystem::directory_iterator(generalBackup)) {
                            std::cout << "[" << backupNumber << "] ";
                            std::cout << entry.path().filename().string() << "\n";
                            backups.emplace(backupNumber, entry.path().filename().string());
                            backupNumber++;
                        }
                        std::cout << "[" << backupNumber << "] " << "Cancel\n";

                        int backupSelection = getSelection();

                        std::filesystem::path modBringerDirectory = std::filesystem::current_path();
                        auto replaceOptions = std::filesystem::copy_options::overwrite_existing;

                        if (settings.operatingSystem == "Windows") {
                            std::filesystem::path saveDirectory = std::filesystem::temp_directory_path();
                            std::filesystem::current_path(saveDirectory);
                            std::filesystem::current_path(R"(../../LocalLow/Flying Oak Games/ScourgeBringer)");
                        }
                        else if (settings.operatingSystem == "Macintosh") {
                            auto home = getenv("HOME");
                            auto directory = std::filesystem::path{ home };
                            std::filesystem::path saveDirectory = directory / "Library/Application Support/Flying Oak Games/ScourgeBringer";
                            std::filesystem::current_path(saveDirectory);
                        }
                        else if (settings.operatingSystem == "Linux") {
                            auto home = getenv("HOME");
                            auto directory = std::filesystem::path{ home };
                            std::filesystem::path saveDirectory = directory / ".scourgebringer";
                            std::filesystem::current_path(saveDirectory);
                        }
                        try {

                            if (exists(generalBackup / backups.at(backupSelection)) && exists(std::filesystem::current_path() / "0.sav")) {
                                std::filesystem::copy(generalBackup / backups.at(backupSelection), std::filesystem::current_path() / "0.sav", replaceOptions);
                                std::filesystem::current_path(modBringerDirectory);
                                std::cout << "Backup " << backups.at(backupSelection) << " restored.\n";
                            }
                        }
                        catch (std::exception e) {
                            std::cout << "User Canceled.\n" << std::endl;
                        }
                    }
                    else {
                        std::cout << "Canceled.\nReturning to main menu.\n\n ";
                    }
                    continue;
                }
                case (2): { // Restore Game Files
                    std::cout << "You want to restore all game files to an unmodded state?\n";
                    std::cout << "[y/n]: ";
                    std::string answer;
                    std::cin >> answer;
                    if (answer == "y") {
                        restoreAll(settings.contentDirectory);
                    }
                    else {
                        std::cout << "Canceled.\nReturning to main menu.\n\n ";
                    }
                    continue;
                }
                case (3): {
                    std::string userProvidedGameDirectory;
                    std::filesystem::path userProvidedPath;
                    do {
                        std::cout << "Please specify game directory:\nExample:\nC:\\Program Files (x86)\\Steam\\steamapps\\common\\ScourgeBringer\n";
                        std::cin.ignore(100, '\n');
                        getline(std::cin, userProvidedGameDirectory);
                        userProvidedPath = userProvidedGameDirectory;
                    } while (!verifyDirectory(userProvidedPath));
                    std::filesystem::path contentFolder = userProvidedPath;
                    std::ofstream config{ "ModBringer.config", std::ios::out | std::ios::in | std::ios::trunc };
                    config << contentFolder << std::endl;
                    settings.gameDirectory = userProvidedPath;
                    settings.contentDirectory = contentFolder;
                    std::cout << "Saving game Directory\n";
                    continue;
                }
                case (4): {
                    // User canceled.
                    std::cout << "Cancelled. Returning to main menu.\n\n";
                    continue;
                } 
                default: {

                    continue;
                }
                };
               

            
            continue;
        
        } case Options::Help : {
            std::cout << "Option " << selection << " selected\n";
            std::cout << "Help topics:\n"
                << "[0] About ModBringer\n"
                << "[1] How to Add New Mods\n" 
                << "[2] How to fix broken save\n"
                << "[3] Cancel\n"
                << std::endl;

            int helpSelection = getSelection();

            switch (helpSelection) {
            case (0): { // About ModBringer

                std::cout << "\nModBringer was created by Squiblydoo and written in C++\n"
                    << "If you want to know more, you can contact Squiblydoo through the ScourgeBringer Discord or\n"
                    << "review the ModBringer code on GitHub.\n" << std::endl;

                pause();
                continue;
            } 
            case (1): { // How to add New Mods
                std::cout << "\nIn order to add new mods, put downloaded files into the 'Mods' folder next to\n"
                    << "the ModBringer.exe. The Mod needs to be put in a folder, in the appropriate category.\n"
                    << "ModBringer comes with a few mods automatically. These mods are examples of how to put\n"
                    << "downloaded mods into the appropriate folders.\n"
                    << "After you put the downloaded mod into a folder, run 'Setup' from the main menu.\n" << std::endl;

                pause();

                continue;
            } 
            case (2) :{ // How to fix broken save
                std::cout << "It is unlikely, but possible, that your save could become broken due to Gameplay Mods.\n"
                    << "This can happen in instances where the save data does not match Gameplay files: for example,\n"
                    << "if you uninstall or install a Gameplay Mod while in the middle of a ScourgeBringer run, then\n"
                    << "the data loaded by the save will be unpredictable and may cause unpredictable behavior.\n"
                    << "This can be avoided by not loading Gameplay Mods while the game is during in a run.\n"
                    << "To restore the save file, use the Restore Save File option in the Maintenance menu.\n\n"
                    << "You may also find it wise to back up your save periodically or before trying Gameplay Mods.\n"
                    << "If you use the ModBringer option to restore your save, any data or progress between the time\n"
                    << "that you restore the save and the time that you last ran the Setup Option will be lost. This is\n"
                    << "because ModBringer makes a backup of your save when you update the installed Mods through ModBringer. Since,\n"
                    << "mods could cause unexpected behavior, ModBringer cannot make more frequent saves and an\n"
                    << "old save is better than none.\n" << std::endl;
                pause();
                continue;
            } 
            case (3) :{ // User canceled.
                std::cout << "Cancelled. Returning to main menu.\n\n";
                continue;
            } 
                    default: {
                // Invalid section made
                continue;
            }
            }

            continue;

        }default: {
            continue;
        }
        }
    };
}

void restoreChanged(std::filesystem::path activeDirectory, std::filesystem::path backupDirectory, std::string extension) {
    try {
        const auto copyOptions = std::filesystem::copy_options::overwrite_existing;
        for (auto entry : std::filesystem::directory_iterator(activeDirectory)) {
            if (entry.is_regular_file() && entry.path().extension() == extension) {
                const auto backupEntry = backupDirectory / entry.path().filename().string();
                const auto backupTimeStamp = std::filesystem::last_write_time(backupEntry).time_since_epoch();
                //const auto backupTimeMinutes = std::chrono::duration_cast<std::chrono::minutes>(backupTimeStamp).count();
                const auto entryTimestamp = std::filesystem::last_write_time(entry).time_since_epoch();
                //const auto entryTimestampMinutes = std::chrono::duration_cast<std::chrono::minutes>(entryTimestamp).count();
                if (entryTimestamp != backupTimeStamp) {
                    std::filesystem::copy(backupEntry, activeDirectory, copyOptions);
                    std::cout << "---Restoring: " << backupEntry.filename().string() << "\n";
                }
            }
        }
    }
    catch (std::exception& e) {
        std::cout << "Exception " << e.what() << std::endl;
    }

}

void restoreAll(std::filesystem::path contentDirectory) {
    std::cout << "Restoring Gameplay files \n";
    restoreChanged(contentDirectory, contentDirectory / "Mods" / "Gameplay Mods" / "Default Gameplay", "");
    restoreChanged(contentDirectory, contentDirectory / "Mods" / "Gameplay Mods" / "Default Gameplay", ".ini");
    std::cout << "Restoring Tilesets \n";
    restoreChanged(contentDirectory / "Tilesets", contentDirectory / "Mods" / "Tileset Mods" / "Default Tilesets", ".xnb");
    std::cout << "Restoring changed Skins\n";
    restoreChanged(contentDirectory, contentDirectory / "Mods" / "Skins" / "DefaultSkin", ".xnb");
    std::cout << "Restoring changed backgrounds\n";
    restoreChanged(contentDirectory / "Backgrounds",contentDirectory / "Mods" / "Background Mods" / "Default Backgrounds", ".xnb");
    std::cout << "Restoring changed Misc Images\n";
    restoreChanged(contentDirectory / "Doors", contentDirectory / "Mods" / "Misc Images Mods" / "Default Misc Images", ".xnb");
    std::cout << "Restoring changed Language Files\n";
    restoreChanged(contentDirectory / "Localizations", contentDirectory / "Mods" / "Language File Mods" / "Default Language Files", "");
    std::cout << "Restoring changed Bosses\n";
    restoreChanged(contentDirectory / "Bosses", contentDirectory / "Mods" / "Boss Mod Images" / "Default Bossess", ".xnb");
    std::cout << "Restoring Fonts\n";
    restoreChanged(contentDirectory / "Fonts", contentDirectory / "Mods" / "Modded Fonts" / "Default Fonts", ".xnb");
}

bool verifyDirectory(std::filesystem::path directory) {
    try {
        directory.make_preferred();
        bool result;
        (!exists(directory)) ? result = false : result = true;
        return result;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what();
        return false;
    }
}

void handleModSelection(std::filesystem::path contentPath, std::filesystem::path activeFilePath, std::filesystem::path backupDirectory, std::string fileType, std::map<int, std::string> map, int userSelection) {
    //Get Mod Category
    auto selectedModDirectory = contentPath / "Mods" / map.at(userSelection);
    std::map<int, std::string> modListing;
    int modNumber{};
    for (auto entry : std::filesystem::directory_iterator(selectedModDirectory)) {
        std::cout << "[" << modNumber << "] ";
        std::cout << entry.path().filename().string() << "\n";
        modListing.emplace(modNumber, entry.path().filename().string());
        modNumber++;
    }
    std::cout << "[" << modNumber << "] Cancel\n";
    int modSelection{};
    modSelection = getSelection();
    try {
        std::cout << "You want to replace the current " << map.at(userSelection) << " with " << modListing.at(modSelection) << "?\n";
        std::cout << "[y/n] ";
        std::string answer;
        std::cin >> answer;
        if (answer == "y") {
            const auto replaceOptions = std::filesystem::copy_options::overwrite_existing;
            auto selectedMod = selectedModDirectory / modListing.at(modSelection);
            restoreChanged(activeFilePath, backupDirectory, fileType); 
            if (selectedMod != backupDirectory) 
            {
                for (auto& entry : std::filesystem::directory_iterator{ selectedMod }) {
                    const auto file = std::filesystem::path(selectedMod / entry);
                    std::filesystem::copy(file, activeFilePath, replaceOptions);
                    std::cout << "Copying: " << file.filename().string() << "\n";
                }
            };
        }
    }
    catch (std::exception) {
        std::cout << "Canceled.\nReturning to main menu.\n\n ";
    }

}


int getSelection() {
    int output;
    std::string input;
    while (true) {
        std::cout << "Please make a selection: " << std::endl;
        std::cin.ignore(100, '\n');
        std::cin >> input;
        std::stringstream ss(input);

        if (ss >> output && !(ss >> input)) return output;
        // Checks for valid conversion to integer and checks for unconverted input
        std::cin.clear();
        std::cerr << "\nInvalid input. Please try again.\n";

    }
}


void pause()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string dummy;
    std::cout << "Press the 'Enter' key to continue . . .";
    std::getline(std::cin, dummy);
}
