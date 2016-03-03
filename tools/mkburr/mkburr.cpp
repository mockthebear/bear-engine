#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <dirent.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

std::vector<std::string> GetFiles(std::string path){
    std::vector<std::string> data;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != NULL) {
      while ((ent = readdir (dir)) != NULL) {
        int size = strlen(ent->d_name);
        bool found = false;
        for (int i=0;i<size;i++){
            if (ent->d_name[i] == '.' && size > 2){
                found = true;
                break;
            }
        }
        if (found){
            std::string add = std::string(ent->d_name);
            data.emplace_back(add);
        }
      }
      closedir (dir);
    } else {
      std::cout << "Dir not found\n";
    }
    return data;
}
class File_f{
    public:
        uint32_t size;
        uint32_t position;
        char *data;
        std::string name;
};
class Structure{
    public:
        uint16_t fileCount;
        std::vector<File_f> files;
};
int main(int argc, char *argv[]){
    if (argc == 1){
        std::cout << "no input\n";
        return 0;
    }
    std::string path = ".";
    std::string outname = "a";

    for (int i=1;i<argc;i++){
        std::string param = argv[i];
        if (param == "-p"){
            i++;
            if (i >= argc){
                std::cout << "use -p like:\nmkburr -p data/folder/\n";
                return 1;
            }
            path = argv[i];
            if (path.substr(path.size()-1) != "/"){
                std::stringstream pname;
                pname << path << "/";
                path = pname.str();
            }
        }else if (param == "-o"){
            i++;
            if (i >= argc){
                std::cout << "use -o like:\nmkburr -o name\n";
                return 1;
            }
            outname = argv[i];
        }
    }

    std::vector<std::string> files = GetFiles(path);
    if (files.size() == 0){
        std::cout << "dir is empty\n";
        return 1;
    }
    std::cout << "packing " << files.size() << " files at ["<<path<<"]\n";

    Structure out;
    out.fileCount = files.size();
    int fileOffset = 0;
    for (auto &it : files){
        File_f dis;
        dis.name = it;
        dis.position = fileOffset;
        std::stringstream fname;
        fname << path << it.c_str();
        FILE *fp = fopen(fname.str().c_str(),"rb");
        if (!fp){
            std::cout << "cant open read "<< it<<"\n";
            return 1;
        }

        fseek(fp, 0L, SEEK_END);
        dis.size = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        std::cout << "file ["<<it<<"] "<<fileOffset<<":"<<(fileOffset+dis.size)<<" ("<<dis.size<<" bytes) \n";
        fileOffset+= dis.size;
        dis.data = new char[dis.size+1];
        for (int i=0;i<dis.size;i++)
            dis.data[i] = fgetc(fp);
        fclose(fp);
        out.files.emplace_back(dis);
    }
    int headerCounter=0;
    std::stringstream S;
    S << outname << ".burr";
    FILE *fout = fopen(S.str().c_str(),"wb");
    if (!fout){
        std::cout << "cant open write done.burr\n";
        return 1;
    }

    fwrite ( &out.fileCount, sizeof(uint16_t), 1,fout );
    headerCounter+=2;

    for (auto &it : out.files){
        fwrite ( &it.size, sizeof(uint32_t), 1,fout );
        headerCounter+=4;
        fwrite ( &it.position, sizeof(uint32_t), 1,fout );
        headerCounter+=4;
        uint8_t fnameSize = it.name.size();
        fwrite ( &fnameSize, sizeof(uint8_t), 1,fout );
        headerCounter+=1;
        fprintf(fout,"%s",it.name.c_str());
        headerCounter+=fnameSize;

    }
    std::cout << "Header has " << headerCounter << " bytes\n";
    for (auto &it : out.files){
        for (int i=0;i<it.size;i++){
            fputc(it.data[i],fout);
        }
    }
    fclose(fout);
    return 0;
}
