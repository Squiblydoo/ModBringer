// ModBringer.cpp 
#
#include <iostream>
#include <iterator>
#include <filesystem>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <chrono>

enum class Options {
    Exit,
    Setup,
    Modifications,
    Maintenance,
    Help
};

struct Settings {
    std::filesystem::path gameDirectory;
};


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

void restoreChanged(std::filesystem::path activeDirectory, std::filesystem::path backupDirectory) {
    try {
        const auto copyOptions = std::filesystem::copy_options::overwrite_existing;
        for (auto const entry : std::filesystem::directory_iterator(activeDirectory)) {
            if (entry.path().extension() == ".xnb") {
                const auto backupEntry = backupDirectory / entry.path().filename().string();
                const auto backupTimeStamp = std::filesystem::last_write_time(backupEntry).time_since_epoch();
                const auto backupTimeMinutes = std::chrono::duration_cast<std::chrono::minutes>(backupTimeStamp).count();
                const auto entryTimestamp = std::filesystem::last_write_time(entry).time_since_epoch();
                const auto entryTimestampMinutes = std::chrono::duration_cast<std::chrono::minutes>(entryTimestamp).count();
                if (entryTimestamp != backupTimeStamp) {
                   std::filesystem::copy(backupEntry, activeDirectory, copyOptions);
                  std::cout << "Restoring: " << backupEntry.filename().string() << "\n";
                }
            }   
        }
    }
    catch (std::exception& e) {
        std::cout << "Exception " << e.what();
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
                                                                                          
    Options optionNumber = Options::Exit;
    int selection;
    Settings settings;


    // Check for existing configuration file; if it exists, read in the current settings
    // TO DO:
    // Check Plausibility of creating a config file using Boost::property_tree and then read and write to tree as needed

    const auto modBringer_path = std::filesystem::current_path();
    //settings.gameDirectory = std::filesystem::path{ R"(C:/Program Files (x86)\Steam\steamapps\common\ScourgeBringer\Content\Mods)" };
    //std::filesystem::path modFolder = settings.gameDirectory;

    if (exists(modBringer_path / "ModBringer.config")) {
        std::ifstream config{ "ModBringer.config" };
        std::cout << "Current Settings: \n";
        std::string savedGameDirectory;
        std::getline(config, savedGameDirectory);
        savedGameDirectory.erase(remove(savedGameDirectory.begin(), savedGameDirectory.end(), '\"'), savedGameDirectory.end());
        settings.gameDirectory = savedGameDirectory;
        std::cout << "Game Directory: " << savedGameDirectory ;
        std::cout << std::endl;
    }
    

    while (true) {
        std::cout << "Type number to section an option.\nOptions\n"
            "[0] Exit\n"
            "[1] Setup or Update\n"
            "[2] Select Modifications \n"
            "[3] ModBringer Maintenance\n"
            "[4] Help" << std::endl;

        std::cin >> selection;
        Options userSelected = static_cast<Options>(selection);


        switch (userSelected) {
        case Options::Exit: {
            std::cout << "Option 0 selected\nExiting." << std::endl;
            std::exit(0);
            break;

        } case Options::Setup: {
            std::cout << "Option 1 selected.\n";
            if (!verifyDirectory(settings.gameDirectory)) { // Game directory not set

                //Check common places first; if they fail, ask the user.
                auto defaultWindowsPath = std::filesystem::path{ R"(C:\Program Files (x86)\Steam\steamapps\common\ScourgeBringer\)" };
                auto defaultMacPath = std::filesystem::path{ R"(~/Library/Application\ Support/Steam/steamapps/common/ScourgeBringer)" };
                    if (verifyDirectory(defaultWindowsPath)) {
                        std::filesystem::path contentFolder = defaultWindowsPath / "Content";
                        std::ofstream config{ "ModBringer.config", std::ios::out | std::ios::in | std::ios::trunc };
                        config << defaultWindowsPath << std::endl;
                        settings.gameDirectory = contentFolder;
                    }
                    else if (verifyDirectory(defaultMacPath)) {
                        std::filesystem::path contentFolder = defaultMacPath / "Content";
                            std::ofstream config{ "ModBringer.config", std::ios::out | std::ios::in | std::ios::trunc };
                            config << defaultWindowsPath << std::endl;
                            settings.gameDirectory = contentFolder;
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
                        std::filesystem::path contentFolder = userProvidedPath / "Content";
                        std::ofstream config{ "ModBringer.config", std::ios::out | std::ios::in | std::ios::trunc };
                        config << contentFolder << std::endl;
                        settings.gameDirectory = contentFolder;
                        std::cout << "Saving game Directory\n";
                    };

            }
            // { R"(C:\Program Files (x86)\Steam\steamapps\common\ScourgeBringer\Content)" }
            
            
                // Try to set a directory

            // Check to see if ScourgeBringer is installed on C in the default directory
            const auto contentFolder = settings.gameDirectory;
            const auto modFolder = settings.gameDirectory / "Mods";
            const auto modFiles = modBringer_path / "Mods";


            // If the mod directory exists, we just need to update it with the items in the directory next to ModBringer.
            const auto copyOptions = std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive;
            if (exists(modFiles) && exists(contentFolder)) {
                std::filesystem::copy(modFiles, modFolder, copyOptions);
            }


            // Ensure backups of original skins exist
            const auto skinDirectory = modFolder / "Skins";
            const auto backupDirectory = skinDirectory / "Default Skin";


            // Make backups if the Backup directory does not exist.
            if (!verifyDirectory(backupDirectory)) {

                // Creates Backup Directory:
                std::filesystem::create_directory(backupDirectory);

                // Creates Backups of "common_X.xnb" files

                const auto skinDirectory = modFolder / "Skins";
                const auto backupSkinDirectory = skinDirectory / "Default Skin";
                for (auto entry : std::filesystem::directory_iterator(contentFolder)) {
                    if (entry.is_regular_file() && entry.path().has_extension()) {
                        std::filesystem::copy(entry, backupSkinDirectory);
                    };
                };


                // Creates backups of Catalog files
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


                if (!exists(generalBackup)) std::filesystem::create_directory(generalBackup);
                // TODO:
                // Find out if this is a stupid way to do this. It seems to work at the moment.
                std::filesystem::path saveDirectory = std::filesystem::temp_directory_path();
                std::filesystem::path modBringerDirectory = std::filesystem::current_path();
                std::filesystem::current_path(saveDirectory);
                std::filesystem::current_path("../../LocalLow/Flying\ Oak\ Games/ScourgeBringer");
                std::filesystem::copy(std::filesystem::current_path() / "0.sav", generalBackup);
                std::filesystem::current_path(modBringerDirectory);


                // Backup Catalog files
                if (!exists(defaultGameplay)) {
                    std::filesystem::create_directory(gameplayMods);
                    std::filesystem::create_directory(defaultGameplay);

                    //  Catalog files don't have an extension but are a regular file
                    for (auto entry : std::filesystem::directory_iterator(contentFolder)) {
                        if (entry.is_regular_file() && !entry.path().has_extension()) {
                            std::filesystem::copy(entry, defaultGameplay);
                        }
                    };
                }

                // Backup Tilesets
                if (!exists(defaultTilesets)) {
                    std::filesystem::create_directory(tilesetDirectory);
                    std::filesystem::create_directory(defaultTilesets);
                    for (auto entry : std::filesystem::directory_iterator(contentFolder / "Tilesets")) {
                        std::filesystem::copy(entry, defaultTilesets);
                    }

                }

                // Backup Boss Image Files
                if (!exists(defaultBosses)) {
                    std::filesystem::create_directory(bossMods);
                    std::filesystem::create_directory(defaultBosses);
                    for (auto entry : std::filesystem::directory_iterator(contentFolder / "Bosses")) {
                        std::filesystem::copy(entry, defaultBosses);
                    }
                }

                // Backup Localization files
                if (!exists(defaultLanguageFiles)) {
                    std::filesystem::create_directory(languageFileMods);
                    std::filesystem::create_directory(defaultLanguageFiles);
                    for (auto entry : std::filesystem::directory_iterator(contentFolder / "Localizations")) {
                        std::filesystem::copy(entry, defaultLanguageFiles);
                    }
                }

                // Backup Misc Files
                if (!exists(defaultMiscImages)) {
                    std::filesystem::create_directory(miscImageMods);
                    std::filesystem::create_directory(defaultMiscImages);
                    for (auto entry : std::filesystem::directory_iterator(contentFolder / "Doors")) {
                        std::filesystem::copy(entry, defaultMiscImages);
                    }
                }

                // Backup background images
                if (!exists(defaultBackgrounds)) {
                    std::filesystem::create_directory(backgroundMods);
                    std::filesystem::create_directory(defaultBackgrounds);
                    for (auto entry : std::filesystem::directory_iterator(contentFolder / "Backgrounds")) {
                        std::filesystem::copy(entry, defaultBackgrounds);
                    }
                }

            }



            break;

        } case Options::Modifications: {
            std::cout << "Option 2 selected.\n\n";
            if (!verifyDirectory(settings.gameDirectory / "Mods")) {
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                    << "!!!Mod directory does not exist. Please perform setup first.!!!\n"
                    << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                    << std::endl;
                break;
            };
            if (exists(settings.gameDirectory / "Mods")) {

                // Iterate through the possible Mod Categories
                int entryNum{ 0 };
                //int modSelection;
                std::map<int, std::string> modTypes;
                int typeNum{ 0 };
                for (const auto entry : std::filesystem::directory_iterator(settings.gameDirectory / "Mods")) {
                    // Exclude the backup folder from the Mod list. It is in this directory for convienance. 
                    if (entry.path().filename().string() == "Backups") continue;

                    std::cout << "[" << typeNum << "] ";
                    std::cout << entry.path().filename().string() << "\n";
                    modTypes.emplace(typeNum, entry.path().filename().string());
                    typeNum++;
                }
                modTypes.emplace(typeNum, "Cancel");
                std::cout << "[" << typeNum << "] Cancel" << std::endl;
                

                //std::cout << "Please make a selection.\n";
                //std::cin >> modSelection ;
                int modSelection = getSelection();
                


                try {
                    std::cout << modTypes.at(modSelection) << "\n";

                    if (modTypes.at(modSelection) == "Cancel") {
                        std::cout << "Canceled.\nReturning to main menu.\n\n ";
                        break;
                    }

                    if (modTypes.at(modSelection) == "GameplayMods") {
                        auto gameplayModDirectory = settings.gameDirectory / "Mods" / "GameplayMods";
                        std::map<int, std::string> gameplayMods;
                        int gameplayModNum{ 0 };
                        for (const auto entry : std::filesystem::directory_iterator(gameplayModDirectory)) {
                            std::cout << "[" << gameplayModNum << "] ";
                            std::cout << entry.path().filename().string() << "\n";
                            gameplayMods.emplace(gameplayModNum, entry.path().filename().string());
                            gameplayModNum++;
                        }

                        if (gameplayModNum == 0) {
                            // This should not happen due to the backups, but we'll do it anyway.
                            std::cout << "There are no Gameplay mods available." << std::endl;
                        }

                        std::cout << "[" << gameplayModNum << "] Cancel\n";
                        int gameplayModSelection;
                        std::cout << "Please make a selection." << std::endl;
                        std::cin >> gameplayModSelection;
                        try {
                            gameplayMods.at(gameplayModSelection);


                        }
                        catch (std::exception) {
                            std::cout << "Canceled.\nReturning to main menu.\n\n ";
                        }

                    };


                    //Handle Tileset Selection
                    if (modTypes.at(modSelection) == "Tileset Mods") {
                        auto tilesetDirectory = settings.gameDirectory / "Mods" / modTypes.at(modSelection);
                        std::map<int, std::string> tilesets;
                        int tilesetNum{ 0 };
                        for (const auto entry : std::filesystem::directory_iterator(tilesetDirectory)) {
                            std::cout << "[" << tilesetNum << "] ";
                            std::cout << entry.path().filename().string() << "\n";
                            tilesets.emplace(tilesetNum, entry.path().filename().string());
                            tilesetNum++;
                        }
                        
                        if (tilesetNum == 0) {
                            // This should not happen due to the backups, but we'll do it anyway.
                            std::cout << "There are no tileset mods available." << std::endl;
                        }

                        std::cout << "[" << tilesetNum << "] Cancel\n";
                        int tilesetSelection;
                        std::cout << "Please make a selection." << std::endl;
                        std::cin >> tilesetSelection;
                        try {
                            tilesets.at(tilesetSelection);
                            std::cout << "You want to replace the current Tileset with " << tilesets.at(tilesetSelection) << "?\n";
                            std::cout << "[y/n] ";
                            std::string answer;
                            std::cin >> answer;
                            if (answer == "y") {
                                std::cout << "Copying Tileset\n";
                                const auto replaceOptions = std::filesystem::copy_options::overwrite_existing;
                                auto selectedTilesetDirectory = tilesetDirectory / tilesets.at(tilesetSelection);
                                auto backupTilesetDirectory = tilesetDirectory / "Default Tilesets";
                                auto activeTileset = settings.gameDirectory / "Mods" / "../Tilesets";
                                restoreChanged(activeTileset, backupTilesetDirectory);
                                if (selectedTilesetDirectory != backupTilesetDirectory) // restoredChange(ap,bp) restored the files, don't copy 
                                {
                                    for (const auto& entry : std::filesystem::directory_iterator{ selectedTilesetDirectory }) {
                                        const auto file = std::filesystem::path(selectedTilesetDirectory / entry);
                                        if (file.extension() == ".xnb") {
                                            std::filesystem::copy(file, activeTileset, replaceOptions);
                                            std::cout << "Copying: " << file.filename().string() << "\n";
                                        }
                                    }
                                };

                            }
                            else {
                                std::cout << "Canceled.\nReturning to main menu.\n\n ";
                            }

                        }
                        catch (std::exception) {
                            std::cout << "Canceled.\nReturning to main menu.\n\n ";
                        }

                    };

                    // Handle Skin Selection
                    if (modTypes.at(modSelection) == "Skins") {


                        const auto skinDirectory = settings.gameDirectory / "Mods" / "Skins";
                        std::cout << "\nSkin options available: \n";
                        std::map<int, std::string> skins;
                        int skinNum{ 0 };
                        for (const auto entry : std::filesystem::directory_iterator(skinDirectory)) {
                            std::cout << "[" << skinNum << "] ";
                            std::cout << entry.path().filename().string() << "\n";
                            skins.emplace(skinNum, entry.path().filename().string());
                            skinNum++;
                        }

                        std::cout << "[" << skinNum << "] Cancel\n" ;

                        int skinSelection = getSelection();
                        try {
                            skins.at(skinSelection);
                            std::cout << "You want to replace the current skin with " << skins.at(skinSelection) << "?\n";
                            std::cout << "[y/n]: ";
                            std::string answer;
                            std::cin >> answer;
                            if (answer == "y") {
                                std::cout << "Copying skin.\n";

                                const auto replaceOptions = std::filesystem::copy_options::overwrite_existing;
                                auto selectedSkinDirectory = skinDirectory / skins.at(skinSelection);
                                auto contentFolder = settings.gameDirectory / "Mods" / "..";
                                std::filesystem::absolute(contentFolder);
                                const auto defaultSkin = settings.gameDirectory / "Mods" / "Skins\\Default Skin";
                                restoreChanged(contentFolder, defaultSkin);
                                if (selectedSkinDirectory != defaultSkin) { // restoredChange(ap,bp) restored the files, don't copy
                                    for (const auto& entry : std::filesystem::directory_iterator{ selectedSkinDirectory }) {
                                        const auto file = std::filesystem::path(selectedSkinDirectory / entry);
                                        if (file.extension() == ".xnb") {
                                            std::filesystem::copy(file, settings.gameDirectory / "Mods" / "..", replaceOptions);
                                            std::cout << "Copying: " << file.filename().string() << "\n";
                                        }
                                    };
                                
                                }
                                
                            }
                            else {
                                std::cout << "Canceled.\nReturning to main menu.\n\n ";
                            }
                        }
                        catch (std::exception&) {
                            std::cout << "Canceled.\nReturning to main menu.\n\n ";
                        }
                    }
                
                
                
                }
                catch (std::exception& e) {
                    std::cout << "Exception: " << e.what();
                }

            }
            break;

        } case Options::Maintenance : {
            std::cout << "Option 3 selected\n";

                if (!verifyDirectory(settings.gameDirectory / "Mods")) {
                    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                        << "!!!Mod directory does not exist. Please perform setup first.!!!\n"
                        << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                        << std::endl;
                    break;
                }

                std::cout << "[0] Return to main menu\n"
                    << "[1] Delete Mod Folder\n"
                    << "[2] Restore Save file to point before ModBringer was installed\n"
                    << "[3] Restore to default game\n"
                    << "[4] Specify different game directory\n"
                    << std::endl;
                int maintenanceSelection = getSelection();
                if (maintenanceSelection == 0) { //Return to main menu.

                    break;
                }
            if (maintenanceSelection == 1) { // Delete Mod folder.
                std::cout << "You want to remove the Mod Folder and all mods?\n";
                std::cout << "[y/n]: ";
                std::string answer;
                std::cin >> answer;
                if (answer == "y") {
                    // TO DO:
                    // Ask the user if they want all backups put back in place
                    // This will include their Save File  
                    std::cout << "Removing Mod Folder.\n To use Mods again, run Setup from the main menu." << std::endl;
                    std::filesystem::remove_all(settings.gameDirectory / "Mods");
                }
                else {
                    std::cout << "Canceled.\nReturning to main menu.\n\n ";
                }
            }

            
            break;
        
        } case Options::Help : {
            std::cout << "Option 4 selected\n";
            std::cout << "Help topics:\n"
                << "[0] Return to main menu\n"
                << "[1] About ModBringer\n" 
                << "[2] How to Add New Mods\n"
                << "[3] How to fix broken save\n"
                << std::endl;

            int helpSelection = getSelection();

            switch (helpSelection) {
            case (0): { // Return to Main Menu
                
                continue;

            } case (1): {
                // TO DO: Provide useful information here.

                break;
            } default: {
                // Invalid section made
                break;
            }
            }

            break;

        }default: {
            break;
        }
        }
    };
}
