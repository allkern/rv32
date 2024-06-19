#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <cstdio>

struct instruction {
    std::string mask;
    std::string match;
    std::string name;
};

struct format {
    std::string mask;
    std::vector <instruction> instructions;
};

void read(const std::string& s, std::string& d) {
    for (char c : s) {
        if (isspace(c))
            break;

        d.push_back(c);
    }
}

int main(int argc, const char* argv[]) {
    std::ifstream file(argv[1]);
    std::string line;

    std::vector <format> formats;

    instruction i;

    int icount = 0;
    int fcount = 0;

    while (!file.eof()) {
        file >> i.mask;
        file >> i.match;
        file >> i.name;

        format* fmt = nullptr;

        auto it = std::find_if(formats.begin(), formats.end(), [i](auto& f) {
            return f.mask == i.mask;
        });

        if (it == formats.end()) {
            formats.push_back({i.mask, {}});

            ++fcount;

            fmt = &formats.back();
        } else {
            fmt = &(*it);
        }

        ++icount;

        fmt->instructions.push_back(i);
    }

    int c = 0;

    for (const auto& fmt : formats) {
        printf("switch (opcode & 0x%s) {\n", fmt.mask.c_str());

        for (const auto& i : fmt.instructions) {
            printf("    case 0x%s: rv32_i_%s(cpu); break;\n",
                i.match.c_str(),
                i.name.c_str()
            );
        }

        puts("}\n");
        
        ++c;
    }

    printf("// Found %d instructions in %u formats\n",
        icount, fcount
    );

    return 0;
}