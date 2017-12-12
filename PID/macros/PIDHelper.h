
/** Some helper functions for PID plots
 *  F.Gaede, DESY, 2017
 */

#include <string>

/** return the path prefix to the directory that contains the given file name
 *  Examples:   "./" if current working directory (i.e. s is filename only)
 *              "../Results/"  for  "../Results/PID_tree.root"
 */
std::string getPathPrefix(const std::string& fileName) {

  char sep = '/';
  
  std::string path = "./" ;

  size_t i = fileName.rfind(sep, fileName.length());
  if (i != std::string::npos) {
    path = fileName.substr(0, i+1 ) ;
  }
  
  return path ;
}
