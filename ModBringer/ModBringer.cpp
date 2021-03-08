// ModBringer.cpp 
#
#include <iostream>
#include <iterator>
#include <filesystem>
#include <string>
#include <map>
#include <fstream>
#include <chrono>


using namespace std;
// TO DO: Make this path Relative and OS agnositic and drive agnostic
// Alternatively, Save path to file if user has to manually specify directory
// At Some point, all paths should really become Relative
filesystem::path modFolder{ R"(C:/Program Files (x86)\Steam\steamapps\common\ScourgeBringer\Content\Mods)" };

enum class Options {
    Exit,
    Setup,
    Modifications,
    Maintenance,
    Help
};

struct Settings {
    filesystem::path gameDirectory;
    string currentSkin = "";
    string tileset = "";
};


bool verifyDirectory(filesystem::path directory) {
    try {
        directory.make_preferred();
        bool result;
        (!exists(directory)) ? result = false : result = true;
        return result;

    }
    catch (const exception& e) {
        cerr << "Error: " << e.what();
        return false;
    }
}
/*
void checkSetting(const string str) {
    
};

void writeSetting(string str) {
    
};
*/

void restoreChanged(filesystem::path activeDirectory, filesystem::path backupDirectory) {
    try {


        const auto copyOptions = filesystem::copy_options::overwrite_existing;
        for (auto const entry : filesystem::directory_iterator(activeDirectory)) {
            if (entry.path().extension() == ".xnb") {
                const auto backupEntry = backupDirectory / entry.path().filename().string();
                const auto backupTimeStamp = filesystem::last_write_time(backupEntry).time_since_epoch();
                const auto backupTimeMinutes = std::chrono::duration_cast<std::chrono::minutes>(backupTimeStamp).count();
                const auto entryTimestamp = filesystem::last_write_time(entry).time_since_epoch();
                const auto entryTimestampMinutes = std::chrono::duration_cast<std::chrono::minutes>(entryTimestamp).count();
                if (entryTimestamp != backupTimeStamp) {
                   filesystem::copy(backupEntry, activeDirectory, copyOptions);
                  cout << "Restoring: " << backupEntry.filename().string() << "\n";
                }
            }   
        }
    }
    catch (exception& e) {
        cout << "Exception " << e.what();
    }

}


int main()
{
    cout <<
        " _______  _______  ______   ______   _______ _________ _        _______  _______  _______ \n"
        "(       )(  ___  )(  __  \\ (  ___ \\ (  ____ )\\__   __/( (    /|(  ____ \\(  ____ \\(  ____ )\n"
        "| () () || (   ) || (  \\  )| (   ) )| (    )|   ) (   |  \\  ( || (    \\/| (    \\/| (    )|\n"
        "| || || || |   | || |   ) || (__/ / | (____)|   | |   |   \\ | || |      | (__    | (____)|\n"
        "| |(_)| || |   | || |   | ||  __ (  |     __)   | |   | (\\ \\) || | ____ |  __)   |     __)\n"
        "| |   | || |   | || |   ) || (  \\ \\ | (\\ (      | |   | | \\   || | \\_  )| (      | (\\ (   \n"
        "| )   ( || (___) || (__/  )| )___) )| ) \\ \\_____) (___| )  \\  || (___) || (____/\\| ) \\ \\__\n"
        "|/     \\|(_______)(______/ |/ \\___/ |/   \\__/\\_______/|/    )_)(_______)(_______/|/   \\__/\n"
        "by Squiblydoo\n" << endl;
                                                                                          
    Options optionNumber = Options::Exit;
    int selection;
    Settings settings;


    // Check for existing configuration file; if it exists, read in the current settings
    // TO DO:
    // Check Plausibility of creating a config file using Boost::property_tree and then read and write to tree as needed

    const auto modBringer_path = filesystem::current_path();
    /*
    if (exists(modBringer_path / "ModBringer.config")) {
        ifstream config{ "ModBringer.config" };
        cout << "Current Settings: \n";
        std::string str;
        while (std::getline(config, str)) {
            std::cout << str << "\n";
        };
        cout << endl;
    }
    else {
        ofstream config{ "ModBringer.config", ios::out | ios::app };
        config << "GameDirectory:\n";
        settings.currentSkin = "DefaultSkin";
        config << "Skin: " << settings.currentSkin << "\n";
        settings.tileset = "DefaultTileset";
        config << "Tileset: " << settings.tileset << "\n";
    };
    */

    while (true) {
        cout << "Type number to section an option.\nOptions\n"
            "[0] Exit\n"
            "[1] Setup or Update\n"
            "[2] Select Modifications \n"
            "[3] ModBringer Maintenance\n"
            "[4] Help" << endl;

        cin >> selection;
        Options userSelected = static_cast<Options>(selection);


        switch (userSelected) {
        case Options::Exit: {
            cout << "Option 0 selected\nExiting." << endl;
            std::exit(0);
            break;

        } case Options::Setup: {
            cout << "Option 1 selected.\n";


            //cout << "Creating/Updating Mod Directory: C:/Program Files (x86)\\Steam\\steamapps\\common\\ScourgeBringer\\Content\\Mod\n\n";
            filesystem::path modFolder{ R"(C:/Program Files (x86)\Steam\steamapps\common\ScourgeBringer\Content\Mods)" };
            filesystem::path contentFolder{ R"(C:/Program Files (x86)\Steam\steamapps\common\ScourgeBringer\Content)" };
            const auto modFiles = modBringer_path / "Mods";
           
            if (!verifyDirectory(contentFolder)) {
                cout << "Game files could not be found.\nIf the game is installed, please specify the game directory: \n";
                
                // TO DO: Logic for specifing game directory

                
                // Exit back to menu if unable to find game directory.
                continue;
                
            };

            // If the mod directory exists, we just need to update it with the items in the directory next to ModBringer.
            const auto copyOptions = filesystem::copy_options::update_existing | filesystem::copy_options::recursive;
            if (exists(modFiles) && exists(contentFolder)) {
                filesystem::copy(modFiles, modFolder, copyOptions);
            }


            // Ensure backups of original skins exist
            const auto skinDirectory = modFolder / "Skins";
            const auto backupDirectory = skinDirectory / "Default Skin";


            // Make backups if the Backup directory does not exist.
            if (!verifyDirectory(backupDirectory)) {

                // Creates Backup Directory:
                filesystem::create_directory(backupDirectory);

                // Creates Backups of "common_X.xnb" files

                const auto skinDirectory = modFolder / "Skins";
                const auto backupSkinDirectory = skinDirectory / "Default Skin";
                for (auto entry : filesystem::directory_iterator(contentFolder)) {
                    if (entry.is_regular_file() && entry.path().has_extension()) {
                        filesystem::copy(entry, backupSkinDirectory);
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


                if (!exists(generalBackup)) filesystem::create_directory(generalBackup);
                // TODO:
                // Find out if this is a stupid way to do this. It seems to work at the moment.
                filesystem::path saveDirectory = filesystem::temp_directory_path();
                filesystem::path modBringerDirectory = filesystem::current_path();
                filesystem::current_path(saveDirectory);
                filesystem::current_path("../../LocalLow/Flying\ Oak\ Games/ScourgeBringer");
                filesystem::copy(filesystem::current_path() / "0.sav", generalBackup);
                filesystem::current_path(modBringerDirectory);


                // Backup Catalog files
                if (!exists(defaultGameplay)) {
                    filesystem::create_directory(gameplayMods);
                    filesystem::create_directory(defaultGameplay);

                    //  Catalog files don't have an extension but are a regular file
                    for (auto entry : filesystem::directory_iterator(contentFolder)) {
                        if (entry.is_regular_file() && !entry.path().has_extension()) {
                            filesystem::copy(entry, defaultGameplay);
                        }
                    };
                }

                // Backup Tilesets
                if (!exists(defaultTilesets)) {
                    filesystem::create_directory(tilesetDirectory);
                    filesystem::create_directory(defaultTilesets);
                    for (auto entry : filesystem::directory_iterator(contentFolder / "Tilesets")) {
                        filesystem::copy(entry, defaultTilesets);
                    }

                }

                // Backup Boss Image Files
                if (!exists(defaultBosses)) {
                    filesystem::create_directory(bossMods);
                    filesystem::create_directory(defaultBosses);
                    for (auto entry : filesystem::directory_iterator(contentFolder / "Bosses")) {
                        filesystem::copy(entry, defaultBosses);
                    }
                }

                // Backup Localization files
                if (!exists(defaultLanguageFiles)) {
                    filesystem::create_directory(languageFileMods);
                    filesystem::create_directory(defaultLanguageFiles);
                    for (auto entry : filesystem::directory_iterator(contentFolder / "Localizations")) {
                        filesystem::copy(entry, defaultLanguageFiles);
                    }
                }

                // Backup Misc Files
                if (!exists(defaultMiscImages)) {
                    filesystem::create_directory(miscImageMods);
                    filesystem::create_directory(defaultMiscImages);
                    for (auto entry : filesystem::directory_iterator(contentFolder / "Doors")) {
                        filesystem::copy(entry, defaultMiscImages);
                    }
                }

                // Backup background images
                if (!exists(defaultBackgrounds)) {
                    filesystem::create_directory(backgroundMods);
                    filesystem::create_directory(defaultBackgrounds);
                    for (auto entry : filesystem::directory_iterator(contentFolder / "Backgrounds")) {
                        filesystem::copy(entry, defaultBackgrounds);
                    }
                }

            }



            break;

        } case Options::Modifications: {
            cout << "Option 2 selected.\n\n";
            if (!verifyDirectory(modFolder)) {
                cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                    << "!!!Mod directory does not exist. Please perform setup first.!!!\n"
                    << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                    << endl;
                break;
            };
            if (exists(modFolder)) {

                // Iterate through the possible Mod Categories
                int entryNum{ 0 };
                int modSelection;
                std::map<int, string> modTypes;
                int typeNum{ 0 };
                for (const auto entry : filesystem::directory_iterator(modFolder)) {
                    // Exclude the backup folder from the Mod list. It is in this directory for convienance. 
                    if (entry.path().filename().string() == "Backups") continue;

                    cout << "[" << typeNum << "] ";
                    cout << entry.path().filename().string() << "\n";
                    modTypes.emplace(typeNum, entry.path().filename().string());
                    typeNum++;
                }

                cout << "Please make a selection.\n";
                cin >> modSelection ;
                try {
                    cout << modTypes.at(modSelection) << "\n";

                    if (modTypes.at(modSelection) == "GameplayMods") {
                        auto gameplayModDirectory = modFolder / "GameplayMods";
                        std::map<int, string> gameplayMods;
                        int gameplayModNum{ 0 };
                        for (const auto entry : filesystem::directory_iterator(gameplayModDirectory)) {
                            cout << "[" << gameplayModNum << "] ";
                            cout << entry.path().filename().string() << "\n";
                            gameplayMods.emplace(gameplayModNum, entry.path().filename().string());
                            gameplayModNum++;
                        }

                        if (gameplayModNum == 0) {
                            // This should not happen due to the backups, but we'll do it anyway.
                            cout << "There are no Gameplay mods available." << endl;
                        }

                        cout << "[" << gameplayModNum << "] Cancel\n";
                        int gameplayModSelection;
                        cout << "Please make a selection." << endl;
                        cin >> gameplayModSelection;
                        try {
                            gameplayMods.at(gameplayModSelection);


                        }
                        catch (std::exception) {
                            cout << "Canceled.\nReturning to main menu.\n\n ";
                        }

                    };


                    //Handle Tileset Selection
                    if (modTypes.at(modSelection) == "Tileset Mods") {
                        auto tilesetDirectory = modFolder / modTypes.at(modSelection);
                        std::map<int, string> tilesets;
                        int tilesetNum{ 0 };
                        for (const auto entry : filesystem::directory_iterator(tilesetDirectory)) {
                            cout << "[" << tilesetNum << "] ";
                            cout << entry.path().filename().string() << "\n";
                            tilesets.emplace(tilesetNum, entry.path().filename().string());
                            tilesetNum++;
                        }
                        
                        if (tilesetNum == 0) {
                            // This should not happen due to the backups, but we'll do it anyway.
                            cout << "There are no tileset mods available." << endl;
                        }

                        cout << "[" << tilesetNum << "] Cancel\n";
                        int tilesetSelection;
                        cout << "Please make a selection." << endl;
                        cin >> tilesetSelection;
                        try {
                            tilesets.at(tilesetSelection);
                            cout << "You want to replace the current Tileset with " << tilesets.at(tilesetSelection) << "?\n";
                            cout << "[y/n] ";
                            string answer;
                            cin >> answer;
                            if (answer == "y") {
                                cout << "Copying Tileset\n";
                                const auto replaceOptions = filesystem::copy_options::overwrite_existing;
                                auto selectedTilesetDirectory = tilesetDirectory / tilesets.at(tilesetSelection);
                                for (const auto& entry : filesystem::directory_iterator{ selectedTilesetDirectory }) {
                                    const auto file = filesystem::path(selectedTilesetDirectory / entry);
                                    if (file.extension() == ".xnb") {
                                        filesystem::copy(file, modFolder / "../Tilesets", replaceOptions);
                                        cout << "Copying: " << file.filename().string() << "\n";
                                    }
                                };

                            }
                            else {
                                cout << "Canceled.\nReturning to main menu.\n\n ";
                            }

                        }
                        catch (std::exception) {
                            cout << "Canceled.\nReturning to main menu.\n\n ";
                        }

                    };

                    // Handle Skin Selection
                    if (modTypes.at(modSelection) == "Skins") {


                        const auto skinDirectory = modFolder / "Skins";
                        cout << "\nSkin options available: \n";
                        std::map<int, string> skins;
                        int skinNum{ 0 };
                        for (const auto entry : filesystem::directory_iterator(skinDirectory)) {
                            cout << "[" << skinNum << "] ";
                            cout << entry.path().filename().string() << "\n";
                            skins.emplace(skinNum, entry.path().filename().string());
                            skinNum++;
                        }

                        cout << "[" << skinNum << "] Cancel\n" ;

                        int skinSelection;
                        cout << "Please make a selection." << endl;
                        cin >> skinSelection;
                        try {
                            skins.at(skinSelection);
                            cout << "You want to replace the current skin with " << skins.at(skinSelection) << "?\n";
                            cout << "[y/n]: ";
                            string answer;
                            cin >> answer;
                            if (answer == "y") {
                                cout << "Copying skin.\n";

                                const auto replaceOptions = filesystem::copy_options::overwrite_existing;
                                auto selectedSkinDirectory = skinDirectory / skins.at(skinSelection);
                                auto contentFolder = filesystem::path { R"(C:/Program Files (x86)\Steam\steamapps\common\ScourgeBringer\Content)" };
                                filesystem::absolute(contentFolder);
                                const auto defaultSkin = modFolder / "Skins\\Default Skin";
                                restoreChanged(contentFolder, defaultSkin);
                                if (selectedSkinDirectory != defaultSkin) {
                                    for (const auto& entry : filesystem::directory_iterator{ selectedSkinDirectory }) {
                                        const auto file = filesystem::path(selectedSkinDirectory / entry);
                                        if (file.extension() == ".xnb") {
                                            filesystem::copy(file, modFolder / "..", replaceOptions);
                                            cout << "Copying: " << file.filename().string() << "\n";
                                        }
                                    };
                                
                                }
                                
                            }
                            else {
                                cout << "Canceled.\nReturning to main menu.\n\n ";
                            }
                        }
                        catch (std::exception&) {
                            cout << "Canceled.\nReturning to main menu.\n\n ";
                        }
                    }
                
                
                
                }
                catch (std::exception& e) {
                    cout << "Exception: " << e.what();
                }

            }
            break;

        } case Options::Maintenance : {
            cout << "Option 3 selected\n";

                if (!verifyDirectory(modFolder)) {
                    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                        << "!!!Mod directory does not exist. Please perform setup first.!!!\n"
                        << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                        << endl;
                    break;
                }

                cout << "[0] Return to main menu\n" 
                << "[1] Delete Mod Folder\n"
            << "Please make a selection.\n";
                int maintenanceSelection{};

            cin >> maintenanceSelection;
            if (maintenanceSelection == 1) {
                cout << "You want to remove the Mod Folder and all mods?\n";
                cout << "[y/n]: ";
                string answer;
                cin >> answer;
                if (answer == "y") {
                    // TO DO:
                    // Ask the user if they want all backups put back in place
                    // This will include their Save File  
                    cout << "Removing Mod Folder.\n To use Mods again, run Setup from the main menu." << endl;
                    filesystem::remove_all(modFolder);
                }
                else {
                    cout << "Canceled.\nReturning to main menu.\n\n ";
                }
            }

            
            break;
        
        } case Options::Help : {
            cout << "Option 4 selected\n";
            cout << "Help topics:\n"
                << "[0] Return to main menu\n"
                << "[1] About ModBringer\n" 
                << "[2] How to Add New Mods\n"
                << "Please make a selection:" << endl;
            int helpSelection{};

            cin >> helpSelection;

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
