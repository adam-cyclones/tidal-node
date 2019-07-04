namespace tidal {
    // Utils
    // I/O

    bool fexists(const std::string& filename) {
    std::ifstream ifile(filename.c_str());
    return (bool)ifile;
    }
    // Error logging
    std::string yellow(std::string str) {
        return "\033[1;33m" + str + "\033[0m";
    }
    std::string red(std::string str) {
        return "\033[1;31m" + str + "\033[0m";
    }
    std::string Error (){
        return red("~ ERR!:");
    }
    std::string Warn (){
        return yellow("~ WARN:");
    }
    std::string Error_resolve_module(const std::string& path_to_file, const std::string& method, const std::string& lua_path = " ") {
        if (method == resolve_L) {
            return Error() + " module '" + path_to_file + "' not found:\nAttempt to resolve file via " + method + "\n" + lua_path + "\n";
        }
        else {
            return Error() + " module '" + path_to_file + "' not found:\nAttempt to resolve file via " + method + "\n";
        }
    }
}