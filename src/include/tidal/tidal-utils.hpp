
using namespace std;


namespace tidal {
    // Utils
    // I/O

    bool fexists(const std::string& filename) {
    ifstream ifile(filename.c_str());
    return (bool)ifile;
    }
    // Error logging
    string yellow(string str) {
        return "\033[1;33m" + str + "\033[0m";
    }
    string red(string str) {
        return "\033[1;31m" + str + "\033[0m";
    }
    string Error (){
        return red("~ ERR!:");
    }
    string Warn (){
        return yellow("~ WARN:");
    }
    string Error_resolve_module(const string path_to_file, const string method, const string lua_path = " ") {
        if (method == resolve_L) {
            return Error() + " module '" + path_to_file + "' not found:\nAttempt to resolve file via " + method + "\n" + lua_path + "\n";
        }
        else {
            return Error() + " module '" + path_to_file + "' not found:\nAttempt to resolve file via " + method + "\n";
        }
    }

}