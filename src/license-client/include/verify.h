#ifndef AA1F2D3E_08F7_460F_B09C_8FA9E37EA234
#define AA1F2D3E_08F7_460F_B09C_8FA9E37EA234

#include <string>

namespace lic
{

bool verify(const std::string& ip, int port=8442, int period=30);
void cleanup();

} // namespace lic

#endif /* AA1F2D3E_08F7_460F_B09C_8FA9E37EA234 */
