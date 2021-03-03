// ModBringer.cpp 
//

#include <iostream>
#include <iterator>
#include <filesystem>
#include <string>
#include <map>

using namespace std;
filesystem::path modFolder{ R"(C:/Program Files (x86)\Steam\steamapps\common\ScourgeBringer\Content\Mods)" };

enum class Options {
    Exit,
    Setup,
    Modifications,
    Maintenance,
    Help
};

enum class Modifications {
    Skins
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


int main()
{
    cout << 
" _______  _______  ______   ______   _______ _________ _        _______  _______  _______ \n"
"(       )(  ___  )(  __  \\ (  ___ \ (  ____ )\\__   __/( (    /|(  ____ \\(  ____ \\(  ____ )\n"
"| () () || (   ) || (  \\  )| (   ) )| (    )|   ) (   |  \\  ( || (    \\/| (    \\/| (    )|\n"
"| || || || |   | || |   ) || (__/ / | (____)|   | |   |   \\ | || |      | (__    | (____)|\n"
"| |(_)| || |   | || |   | ||  __ (  |     __)   | |   | (\\ \\) || | ____ |  __)   |     __)\n"
"| |   | || |   | || |   ) || (  \\ \\ | (\\ (      | |   | | \\   || | \\_  )| (      | (\\ (   \n"
"| )   ( || (___) || (__/  )| )___) )| ) \\ \\_____) (___| )  \\  || (___) || (____/\\| ) \\ \\__\n"
"|/     \\|(_______)(______/ |/ \\___/ |/   \\__/\\_______/|/    )_)(_______)(_______/|/   \\__/\n"
"by Squiblydoo";
                                                                                          
        //"Nexus OS v1\n" << "Reading floppy...\n" << "Starting 'ModBringer' program.\n";
    Options optionNumber = Options::Exit;
    int selection;
    while (true) {
        cout << "Type number to section an option.\nOptions\n"
            "[0] Exit\n"
            "[1] Setup or Update\n"
            "[2] Select Modifications \n"
            "[3] ModBringer Maintenance\n
            "[4] Help\n";

        cin >> selection;
        Options userSelected = static_cast<Options>(selection);


        switch (userSelected) {
        case Options::Exit: {
            cout << "Option 0 selected\nExiting.\n";
            std::exit(0);
            break;

        } case Options::Setup: {
            cout << "Option 1 selected.\n";

            cout << "Creating/Updating Mod Directory: C:/Program Files (x86)\\Steam\\steamapps\\common\\ScourgeBringer\\Content\\Mod\n\n";
            filesystem::path modFolder{ R"(C:/Program Files (x86)\Steam\steamapps\common\ScourgeBringer\Content\Mods)" };
            filesystem::path contentFolder{ R"(C:/Program Files (x86)\Steam\steamapps\common\ScourgeBringer\Content)" };
            const auto modBringer_path = filesystem::current_path();
            const auto modFiles = modBringer_path / "Mods";
            const auto copyOptions = filesystem::copy_options::update_existing | filesystem::copy_options::recursive;

            if (!verifyDirectory(contentFolder)) {
                cout << "Game files could not be found.\nVerify that the game is installed.";
            };

            if (exists(modFiles)) {
                filesystem::copy(modFiles, modFolder, copyOptions);
            }

            break;

        } case Options::Modifications: {
            cout << "Option 2 selected.\n\n";
            if (!verifyDirectory(modFolder)) {
                cout << "Mod directory does not exist. Please perform setup first.\n\n";
                break;
            };
            if (exists(modFolder)) {
                int entryNum{ 0 };
                int modSelection;

                cout << "Please make a selection.\n";

                // Iterate the Mod folder to print the existing mod directories.
                for (const auto& entry : filesystem::directory_iterator{ modFolder }) {
                    cout << "[" << entryNum << "] ";
                    cout << entry.path().filename().string() << "\n";
                    entryNum++;
                }
                cin >> modSelection;
                Modifications mods = static_cast<Modifications>(modSelection);

                switch (mods) {
                case Modifications::Skins: {
                    

                   // Ensure backups of original skins exist.
                    filesystem::path contentFolder{ R"(C:/Program Files (x86)\Steam\steamapps\common\ScourgeBringer\Content)" };
                    const auto skinDirectory = modFolder / "Skins";
                    const auto backupDirectory = skinDirectory / "Default Skin";

                    // Make backup of skins if the Backup directory does not exist.
                    if (!verifyDirectory(backupDirectory)) {  
                        filesystem::create_directory(backupDirectory);
                        string prefix = "common_";
                        string suffix = ".xnb";
                        for (int i{}; i < 15; i++) {
                            string filename = prefix + to_string(i) + suffix;
                            const filesystem::path backup{ contentFolder / filename };
                            filesystem::copy(backup, backupDirectory);
                        }
                    }


                    cout << "\nSkin options available: \n";
                    std::map<int, string> skins;
                    int skinNum{ 0 };
                    for (const auto& entry : filesystem::directory_iterator{ skinDirectory }) {
                        cout << "[" << skinNum << "] ";
                        cout << entry.path().filename().string() << "\n";
                        skins.emplace(skinNum, entry.path().filename().string());
                        skinNum++;
                    }
                    cout << "[" << skinNum << "] Cancel\n";

                    int skinSelection;
                    cout << "Please make a selection.\n";
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
                            for (const auto& entry : filesystem::directory_iterator{ selectedSkinDirectory }) {
                                const auto file = filesystem::path(selectedSkinDirectory / entry);
                                if (file.extension() == ".xnb") {
                                    filesystem::copy(file, contentFolder, replaceOptions);
                                }
                            };

                        }
                        else if (answer == "n") {

                        }
                        else {

                        }
                    }
                    catch (std::exception&) {
                        cout << "Canceled.\nReturning to main menu.\n\n ";
                    }

                    break;
                } default: {
                    break;
                }
                }
            }
            break;

        } case Options::Maintenance : {
            cout << "Option 3 selected\n";
            break;
        
        } case Options::Help : {
            cout << "Option 4 selected\n";
            break;

        }default: {
            break;
        }
        }
    };
}
