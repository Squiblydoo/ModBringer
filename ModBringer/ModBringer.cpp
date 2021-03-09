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
    std::filesystem::path gameDirectory ;
    std::filesystem::path contentDirectory;
};

bool verifyDirectory(std::filesystem::path directory);

void restoreChanged(std::filesystem::path activeDirectory, std::filesystem::path backupDirectory, std::string extension);

int getSelection();

void handleModSelection(std::filesystem::path contentPath, std::filesystem::path activeFilePath, std::filesystem::path backupDirectory, std::string fileType, std::map<int, std::string> map, int userSelection);


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
    const auto modBringer_path = std::filesystem::current_path();

    if (exists(modBringer_path / "ModBringer.config")) {
        std::ifstream config{ "ModBringer.config" };
        std::cout << "Current Settings: \n";
        std::string savedGameDirectory;
        std::getline(config, savedGameDirectory);
        savedGameDirectory.erase(remove(savedGameDirectory.begin(), savedGameDirectory.end(), '\"'), savedGameDirectory.end());
        settings.gameDirectory = savedGameDirectory;
        settings.contentDirectory = settings.gameDirectory / "Content";
        std::cout << "Game Directory: " << savedGameDirectory ;
        std::cout << std::endl;
    }
    

    while (true) {
        std::cout << "\nType number to section an option.\nOptions\n"
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
            continue;

        } case Options::Setup: {
            std::cout << "Option 1 selected.\n";
            if (!verifyDirectory(settings.gameDirectory)) { // Game directory not set

                //Check common places first; if they fail, ask the user.
                auto defaultWindowsPath = std::filesystem::path{ R"(C:\Program Files (x86)\Steam\steamapps\common\ScourgeBringer\)" };
                defaultWindowsPath.make_preferred();
                 if (verifyDirectory(defaultWindowsPath)) {
                        std::filesystem::path contentFolder = defaultWindowsPath / "Content";
                        contentFolder.make_preferred();
                        std::ofstream config{ "ModBringer.config", std::ios::out | std::ios::in | std::ios::trunc };
                        config << defaultWindowsPath << std::endl;
                        settings.gameDirectory = defaultWindowsPath;
                        settings.contentDirectory = contentFolder;
                    } else {
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
           
            const auto contentFolder = settings.contentDirectory;
            const auto modFolder = contentFolder / "Mods";
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
                std::filesystem::current_path(R"(../../LocalLow/Flying Oak Games/ScourgeBringer)");
                std::filesystem::copy(std::filesystem::current_path() / "0.sav", generalBackup);
                std::cout << "Copy of ScourgeBringer save file created.\n";
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

            }



            continue;

        } case Options::Modifications: {
            std::cout << "Option 2 selected.\n\n";
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
                for (const auto entry : std::filesystem::directory_iterator(settings.contentDirectory / "Mods")) {
                    // Exclude the backup folder from the Mod list. It is in this directory for convienance. 
                    if (entry.path().filename().string() == "Backups") continue;

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
                        handleModSelection(settings.contentDirectory, settings.contentDirectory, settings.contentDirectory / "Mods" / "Skins" / "Default Skin", ".xnb", modTypes, modSelection);
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

                }
                catch (std::exception& e) {
                    std::cout << "Exception: " << e.what();
                }

            }
            continue;

        } case Options::Maintenance : {
            std::cout << "Option 3 selected\n";

                if (settings.contentDirectory == "" || !verifyDirectory(settings.contentDirectory / "Mods")) {
                    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                        << "!!!Mod directory does not exist. Please perform setup first.!!!\n"
                        << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                        << std::endl;
                    continue;
                }

                std::cout << "[0] Delete Mod Folder\n"
                    << "[1] Restore Save file to point before ModBringer was installed\n"
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
                        std::cout << "Restoring Gameplay files \n";
                        restoreChanged(settings.contentDirectory, settings.contentDirectory / "Mods" / "Gameplay Mods" / "Default Gameplay", "");
                        std::cout << "Restoring Tilesets \n";
                        restoreChanged(settings.contentDirectory / "Tilesets", settings.contentDirectory / "Mods" / "Tileset Mods" / "Default Tilesets", ".xnb");
                        std::cout << "Restoring changed Skins\n";
                        restoreChanged(settings.contentDirectory, settings.contentDirectory / "Mods" / "Skins" / "Default Skin", ".xnb");
                        std::cout << "Restoring changed backgrounds\n";
                        restoreChanged(settings.contentDirectory / "Backgrounds", settings.contentDirectory / "Mods" / "Background Mods" / "Default Backgrounds", ".xnb");
                        std::cout << "Restoring changed Misc Images\n";
                        restoreChanged(settings.contentDirectory / "Doors", settings.contentDirectory / "Mods" / "Misc Images Mods" / "Default Misc Images", ".xnb");
                        std::cout << "Restoring changed Language Files\n";
                        restoreChanged(settings.contentDirectory / "Localizations", settings.contentDirectory / "Mods" / "Language File Mods" / "Default Language Files", "");
                        std::cout << "Restoring changed Bosses\n";
                        restoreChanged(settings.contentDirectory / "Bosses", settings.contentDirectory / "Mods" / "Boss Mod Images" / "Default Bossess", ".xnb");

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
                        << "Do you want to restore your save to what it was before you installed ModBringer?\n";
                    std::cout << "[y/n]: ";
                    std::string answer;
                    std::cin >> answer;
                    if (answer == "y") {
                        std::filesystem::path generalBackup = settings.contentDirectory / "Mods" / "Backup";
                        std::filesystem::path saveDirectory = std::filesystem::temp_directory_path();
                        std::filesystem::path modBringerDirectory = std::filesystem::current_path();
                        std::filesystem::current_path(saveDirectory);
                        std::filesystem::current_path(R"(../../LocalLow/Flying Oak Games/ScourgeBringer)");
                        if (exists(generalBackup / "0.sav") && exists(std::filesystem::current_path() / "0.sav")) {
                            std::filesystem::copy(generalBackup / "0.sav", std::filesystem::current_path() / "0.sav");
                            std::cout << "Copy of ScourgeBringer save file created.\n";
                            std::filesystem::current_path(modBringerDirectory);
                            std::cout << "Backup restored.\n";
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
                        std::cout << "Restoring Gameplay files \n";
                        restoreChanged(settings.contentDirectory, settings.contentDirectory / "Mods" / "Gameplay Mods" / "Default Gameplay", "");
                        std::cout << "Restoring Tilesets \n";
                        restoreChanged(settings.contentDirectory / "Tilesets", settings.contentDirectory / "Mods" / "Tileset Mods" / "Default Tilesets", ".xnb");
                        std::cout << "Restoring changed Skins\n";
                        restoreChanged(settings.contentDirectory, settings.contentDirectory / "Mods" / "Skins" / "Default Skin", ".xnb");
                        std::cout << "Restoring changed backgrounds\n";
                        restoreChanged(settings.contentDirectory / "Backgrounds", settings.contentDirectory / "Mods" / "Background Mods" / "Default Backgrounds", ".xnb");
                        std::cout << "Restoring changed Misc Images\n";
                        restoreChanged(settings.contentDirectory / "Doors", settings.contentDirectory / "Mods" / "Misc Images Mods" / "Default Misc Images", ".xnb");
                        std::cout << "Restoring changed Language Files\n"; 
                        restoreChanged(settings.contentDirectory / "Localizations", settings.contentDirectory / "Mods" / "Language File Mods" / "Default Language Files","");
                        std::cout << "Restoring changed Bosses\n";
                        restoreChanged(settings.contentDirectory / "Bosses", settings.contentDirectory / "Mods" / "Boss Mod Images" / "Default Bossess", ".xnb");

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
            std::cout << "Option 4 selected\n";
            std::cout << "Help topics:\n"
                << "[0] About ModBringer\n"
                << "[1] How to Add New Mods\n" 
                << "[2] How to fix broken save\n"
                << "[3] Cancel\n"
                << std::endl;

            int helpSelection = getSelection();

            switch (helpSelection) {
            case (0): { // About ModBringer
                std::cout << "ModBringer was created by Squiblydoo and written in C++\n"
                    << "If you want to know more, you can contact Squiblydoo through the ScourgeBringer Discord or\n"
                    << "review the ModBringer code on GitHub." << std::endl;

                system("pause");
                continue;
            } 
            case (1): { // How to add New Mods
                std::cout << "In order to add new mods, put downloaded files into the 'Mods' folder next to\n"
                    << "the ModBringer.exe. The Mod needs to be put in a folder, in the appropriate category.\n"
                    << "ModBringer comes with a few mods automatically. These mods are examples of how to put\n"
                    << "downloaded mods into the appropriate folders.\n"
                    << "After you put the downloaded mod into a folder, run 'Setup' from the main menu." << std::endl;

                system("pause");

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
                    << "that you restore the save and the time that you installed ModBringer, will be lost. This is\n"
                    << "because ModBringer only makes a backup of your save when you install ModBringer. Since,\n"
                    << "mods could cause unexpected behavior, ModBringer cannot make more frequent saves and an\n"
                    << "old save is better than none.\n" << std::endl;
                system("pause");
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
        for (auto const entry : std::filesystem::directory_iterator(activeDirectory)) {
            if (entry.is_regular_file() && entry.path().extension() == extension) {
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
        std::cout << "Exception " << e.what() << std::endl;
    }

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
    for (const auto entry : std::filesystem::directory_iterator(selectedModDirectory)) {
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
            restoreChanged(activeFilePath, backupDirectory, fileType); // Gameplay files have no extension
            if (selectedMod != backupDirectory) // restoredChange(ap,bp) restored the files, don't copy 
            {
                for (const auto& entry : std::filesystem::directory_iterator{ selectedMod }) {
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
