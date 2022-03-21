#include "GameManager.hpp"

GameManager::GameManager() {
    string configPath = "./config";
    string itemConfigPath = configPath + "/item.txt";
    string recipesConfigPath = configPath +"/recipe";
    vector<string> itemConfig = fileManager.readFile(itemConfigPath);
    for(string s : itemConfig) {
        stringstream ss(s);
        string temp[4];
        ss >>temp[0] >>temp[1] >>temp[2] >>temp[3];
        int id = stoi(temp[0]);
        string name = temp[1];
        string type = temp[2];
        string itemType = temp[3];
        mpNameId[name] = id;
        if(itemType=="TOOL") {
            tools.insert(name);
        } else if(itemType=="NONTOOL" && type=="-") {
            nonTools.insert(name);
        } else if(itemType=="NONTOOL") {
            typedNonTools.insert({name,type});
        } else {
            ;
        }
    }
    for(string name: tools) {
        Tool* tool = new Tool(mpNameId[name],name);
        mpIdItem[mpNameId[name]] = tool;
    }    
    for(string name: nonTools) {
        NonTool* nonTool = new NonTool(mpNameId[name],name);
        mpIdItem[mpNameId[name]] = nonTool;
    }
    for(auto [name,type]: typedNonTools) {
        TypedNonTool* typedNonTool = new TypedNonTool(mpNameId[name],name,type);
        mpIdItem[mpNameId[name]] = typedNonTool;
    }
    vector<Recipe> recipes;
    vector<vector<string>> recipesConfig = fileManager.readFiles(recipesConfigPath);
    for(vector<string> recipeConfig: recipesConfig) {
        recipes.push_back(Recipe(recipeConfig));
    }
    craft = new Craft(recipes, mpIdItem, mpNameId);
}

GameManager::~GameManager() {
    vector<Item*> v;
    for(auto [id,item]: mpIdItem) {
        v.push_back(item);
    }
    for(int i=0;i<(int)v.size();i++) {
        delete v[i];
    }
}

void GameManager::play() {
    bool isExit=false;
    while(!isExit) {
        cout <<(*craft) <<"\n";
        cout <<inventory <<"\n";
        vector<string> command = terminalManager.readCommand();
        try{
            if(command[0]=="GIVE") {
                giveCommand(command);
            } else if(command[0]=="DISCARD") {
                discardCommand(command);
            } else if(command[0]=="MOVE") {
                moveCommand(command);
            } else if(command[0]=="CRAFT") {
                craftCommand(command);
            } else if(command[0]=="USE") {
                useCommand(command);
            } else if(command[0]=="EXPORT") {
                exportCommand(command);
            } else {
                throw InvalidCommandException(command[0]);
            }
        } catch(const exception &e) {
            cout << "EXCEPTION: " << e.what() <<"\n";
        }
    }    
}

void GameManager::giveCommand(vector<string> command) {
    try {
        if((int)command.size() == 3) {
            string name = command[1];
            int qty = stoi(command[2]);
            auto nid = mpNameId.find(name);
            if(nid==mpNameId.end()) {
                throw ItemNotFoundException(name);
            }
            auto iditem = mpIdItem.find(nid->second);
            if(iditem==mpIdItem.end()) {
                throw ItemNotFoundException(name);
            }
            inventory.addItem(iditem->second, qty);
        } else {
            throw InvalidCommandException(command[0]);
        }
    } catch(const exception& e) {
        cout <<"EXCEPTION: " <<e.what() <<"\n";
    }
}

void GameManager::discardCommand(vector<string> command) {
    try {
        if((int)command.size() == 3) {
            int qty = stoi(command[2]);
            inventory.remove(command[1], qty);
        } else {
            throw InvalidCommandException(command[0]);
        }
    } catch(const exception& e) {
        cout <<"EXCEPTION: " <<e.what() <<"\n";
    }
}

void GameManager::moveCommand(vector<string> command) {
    try {
        if((int)command.size()>=4) {
            int ctTarget = stoi(command[2]);
            if(ctTarget+3==(int)command.size()) {
                bool srcInvValid, srcCrfValid, allTargetInvValid, allTargetCrfValid;
                srcInvValid = inventory.isInvSlotValid(command[1], ctTarget);
                srcCrfValid = craft->isCrfSlotValid(command[1], ctTarget);
                allTargetInvValid = true;
                allTargetCrfValid = true;
                Item* item;
                // dapetin item src dulu atau keluarin exception command move ga valid
                // cek canBeAdded inventory sm craft.
                if(srcInvValid) {
                    item = inventory[command[1]].getItem()->clone();
                } else if(srcCrfValid) {
                    item = (*craft)[command[1]].getItem()->clone();
                } else {
                    throw InvalidCommandException(command[0]);
                }
                try {
                    for(int i=3;i<(int)command.size();i++) {
                        allTargetInvValid &= inventory.canBeAdded(item,command[i],1);
                        allTargetCrfValid &= craft->canBeAdded(item,command[i],1);
                    }

                    if(srcInvValid && allTargetInvValid) {
                        if(ctTarget==1) {
                            // numpuk sampe target 64
                            while(inventory.isInvSlotValid(command[1],1) && inventory.canBeAdded(item,command[3],1)) {
                                inventory.remove(command[1],1);
                                inventory.addItem(item,command[3],1);
                            }
                        } else {
                            throw InvalidCommandException(command[0]);
                        }
                    } else if(srcInvValid && allTargetCrfValid) {
                        // asumsi craft_slot_id beda semua
                        inventory.remove(command[1],ctTarget);
                        for(int i=3;i<(int)command.size();i++) {
                            craft->addItem(item,command[i],1);
                        }
                    } else if(srcCrfValid && allTargetInvValid) {
                        if(ctTarget==1) {
                            // Dari craft ke Inventory disamakan dengan inventory ke craft yaitu satu item dipindahkan
                            craft->removeItem(command[1],1);
                            inventory.addItem(item,command[3],1);
                        } else {
                            throw InvalidCommandException(command[0]);
                        }
                    } else {
                        throw InvalidCommandException(command[0]);
                    }
                } catch(const exception& e) {
                    delete item;
                    throw; // rethrow original object
                }
            } else {
                throw InvalidCommandException(command[0]);
            }
        } else {
            throw InvalidCommandException(command[0]);
        }
    } catch(const exception& e) {
        cout <<"EXCEPTION: " <<e.what() <<"\n";
    }
}

void GameManager::craftCommand(vector<string> command) {
    try {
        if((int)command.size()==1) {
            // Slot* result = craft->crafting();
            // crafting berhasil, pindahkan result ke inventory seperti give
            // delete result
        } else {
            throw InvalidCommandException(command[0]);
        }
    } catch(const exception& e) {
        cout <<"EXCEPTION: " <<e.what() <<"\n";
    }
}

void GameManager::useCommand(vector<string> command) {
    try {
        if((int)command.size()==2) {
            inventory.use(command[1]);
        } else {
            throw InvalidCommandException(command[0]);
        }
    } catch(const exception& e) {
        cout <<"EXCEPTION: " <<e.what() <<"\n";
    }
}

void GameManager::exportCommand(vector<string> command) {
    try {
        if((int)command.size()==2) {
            fileManager.write(command[1], inventory.exportInventory());
        } else {
            throw InvalidCommandException(command[0]);
        }
    } catch(const exception& e) {
        cout <<"EXCEPTION: " <<e.what() <<"\n";
    }
}