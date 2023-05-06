#include "cleantemp.h"
#include "functions.h"
#include "Globals.h"
int main(int argc, char* argv[])
{
  bool
    dir_tmp_system_exists = false,
    dir_tmp_user_exists = false,
    skip_prompt = false,
    user_only = false,
    system_only = false;
  const std::regex
    RGX_PROMPT("^/([pP]|[pP][rR][oO][mM][pP][tT])$"),
    RGX_HELP("^/([?]|[hH]|[hH][eE][lL][pP])$"),
    RGX_USER("^/([uU]|[uU][sS][eE][rR])$"),
    RGX_SYSTEM("^/([sS]|[sS][yY][sS][tT][eE][mM])$");
  const std::string TITLE = "Clean Temp";
  std::string tmp_dir_user, tmp_dir_sys;
  HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
  SetConsoleMode(hInput, ENABLE_VIRTUAL_TERMINAL_INPUT);
  HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleMode(hOutput, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleTitleA(TITLE.c_str());
  const std::vector <std::string> BOX =
  {
    u8"\u250C", u8"\u2500", u8"\u2510", u8"\u2502",
    u8"\u2514", u8"\u2518", u8"\u251C", u8"\u2524",
    u8"\u2570", u8"\u256F"
  };
  std::string box_line;
  for (auto i = 0; i < 48; ++i)
  {
    box_line += BOX[1];
  }
  std::string header_message;
  if (argc > 1)
  {
    for (auto i = 1; i < argc; i++)
    {
      if (std::regex_match(argv[i], RGX_HELP))
      {
        header_message.clear();
        header_message.append
        (
          BOX[0] + box_line + BOX[2] + '\n' +
          BOX[3] + "                   Clean Temp                   " + BOX[3] + '\n' +
          BOX[6] + box_line + BOX[7] + '\n' +
          BOX[3] + " Clean temporary files and directories.         " + BOX[3] + '\n' +
          BOX[6] + box_line + BOX[7] + '\n' +
          BOX[3] + " cleantemp [/H] [/S] [/U] [/P]                  " + BOX[3] + '\n' +
          BOX[6] + box_line + BOX[7] + '\n' +
          BOX[3] + " /?,/H,/HELP     This help screen.              " + BOX[3] + '\n' +
          BOX[3] + " /P,/PROMPT      Skip [Enter] prompts.          " + BOX[3] + '\n' +
          BOX[3] + " /U,/USER        User temp directory only.      " + BOX[3] + '\n' +
          BOX[3] + " /S,/SYSTEM      System temp directory only.    " + BOX[3] + '\n' +
          BOX[6] + box_line + BOX[7] + '\n' +
          BOX[3] + u8" \u00A9 2022 Ian Pride - New Pride Software/Services " + BOX[3] + '\n' +
          BOX[4] + box_line + BOX[5] + '\n'
        );
        std::cout << header_message;
        if (!skip_prompt)
        {
          WaitCheck();
        }
        return 0;
      }
      if (std::regex_match(argv[i], RGX_PROMPT))
      {
        skip_prompt = true;
        continue;
      }
      if (std::regex_match(argv[i], RGX_USER))
      {
        user_only = true;
        system_only = false;
        continue;
      }
      if (std::regex_match(argv[i], RGX_SYSTEM))
      {
        system_only = true;
        user_only = false;
        continue;
      }
    }
  }
  const DWORD buffSize = 65535;
  static char buffer[buffSize];
  if (!system_only)
  {
    if (GetEnvironmentVariableA("TEMP", buffer, buffSize))
    {
      tmp_dir_user.append(buffer);
      dir_tmp_user_exists = DirectoryExists(tmp_dir_user.c_str());
    }
  }
  if (!user_only)
  {
    if (GetEnvironmentVariableA("SYSTEMROOT", buffer, buffSize))
    {
      tmp_dir_sys.append(buffer);
      tmp_dir_sys.append("\\Temp");
      dir_tmp_system_exists = DirectoryExists(tmp_dir_sys.c_str());
    }
  }
  if (!(dir_tmp_user_exists || dir_tmp_system_exists))
  {
    std::cerr << "No temporary directories seems to exist.\n";
    if (!skip_prompt)
    {
      WaitCheck();
    }
    return 1;
  }
  header_message.append
  (
    '\n' +
    BOX[0] + box_line + BOX[2] + '\n' +
    BOX[3] + "                  Clean Temp                    " + BOX[3] + '\n' +
    BOX[6] + box_line + BOX[7] + '\n' +
    BOX[3] + " Clean all temporary directories for a user.    " + BOX[3] + '\n' +
    BOX[4] + box_line + BOX[5] + "\n\n"
  );
  std::cout << header_message;
  std::vector <std::string> all_user, all_system;
  if ((!user_only) && dir_tmp_system_exists)
  {
    header_message.clear();
    header_message.append
    (
      BOX[0] + box_line + BOX[2] + '\n' +
      BOX[3] + " Found system temp directory and enumerating    " + BOX[3] + '\n' +
      BOX[3] + " children files and directories.                " + BOX[3] + '\n' +
      BOX[4] + box_line + BOX[5] + "\n\n"
    );
    std::cout << header_message;
    try
    {
      for (auto const& entry : std::filesystem::directory_iterator(tmp_dir_sys))
      {
        all_system.push_back(entry.path().string());
      }
    }
    catch (std::filesystem::filesystem_error& ferr)
    {
      std::cerr << ferr.what() << '\n';
      if (!skip_prompt)
      {
        WaitCheck();
      }
      return 2;
    }
    if (!all_system.empty())
    {
      header_message.clear();
      header_message.append
      (
        BOX[0] + box_line + BOX[2] + '\n' +
        BOX[3] + " Found files and folders in the system temp     " + BOX[3] + '\n' +
        BOX[3] + " directory and attempting to delete.            " + BOX[3] + '\n' +
        BOX[4] + box_line + BOX[5] + "\n\n"
      );
      std::cout << header_message;
      for (std::string entry : all_system)
      {
        std::error_code ecode;
        std::filesystem::remove_all(entry, ecode);
        int evalc = (int)ecode.value();
        if (evalc > 0)
        {
          if (evalc == 5)
          {
            std::cerr << "Access is denied for " << entry << std::endl;
          }
          if (evalc == 32)
          {
            std::cerr << entry << " is in use by another process." << std::endl;
          }
        }
        else
        {
          std::cout << entry << " deleted successfully." << std::endl;
        }
      }
    }
  }
  if ((!system_only) && dir_tmp_user_exists)
  {
    header_message.clear();
    header_message.append
    (
      BOX[0] + box_line + BOX[2] + '\n' +
      BOX[3] + "  Found user temp directory and enumerating     " + BOX[3] + '\n' +
      BOX[3] + "  children files and directories.               " + BOX[3] + '\n' +
      BOX[4] + box_line + BOX[5] + "\n\n"
    );
    std::cout << header_message;
    for (auto const& entry : std::filesystem::directory_iterator(tmp_dir_user))
    {
      all_user.push_back(entry.path().string());
    }
    if (!all_user.empty())
    {
      header_message.clear();
      header_message.append
      (
        BOX[0] + box_line + BOX[2] + '\n' +
        BOX[3] + " Found files and folders in the user temp       " + BOX[3] + '\n' +
        BOX[3] + " directory and attempting to delete.            " + BOX[3] + '\n' +
        BOX[4] + box_line + BOX[5] + "\n\n"
      );
      std::cout << header_message;
      for (auto entry : all_user)
      {
        std::error_code ecode;
        std::filesystem::remove_all(entry, ecode);
        int evalc = (int)ecode.value();
        if (evalc > 0)
        {
          if (evalc == 5)
          {
            std::cerr << "Access is denied for " << entry << std::endl;
          }
          if (evalc == 32)
          {
            std::cerr << entry << " is in use by another process." << std::endl;
          }
        }
        else
        {
          std::cout << entry << " deleted successfully." << std::endl;
        }
      }
    }
  }
  if (!skip_prompt)
  {
    WaitCheck();
  }
}
