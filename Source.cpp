#include <iostream>
#include <string>
#include <vector>
#include <codecvt>
#include <filesystem>
#include <windows.h>
#include <png.h>
#include <lua.hpp>

void error(std::string str) {
  std::cerr << str << std::endl;
}

void error(std::wstring wstr) {
  std::wcerr << wstr << std::endl;
}

void print(std::string str) {
  std::cout << str << std::endl;
}

void print(std::wstring wstr) {
  std::wcout << wstr << std::endl;
}

int main() {
  std::wstring file_name;
  std::cout << "Enter your png image path: ";
  std::wcin >> file_name;
  int utf8Size = WideCharToMultiByte(CP_UTF8, 0, file_name.c_str(), -1, nullptr, 0, nullptr, nullptr);
  std::string utf8Str(utf8Size, '\0');
  WideCharToMultiByte(CP_UTF8, 0, file_name.c_str(), -1, &utf8Str[0], utf8Size, nullptr, nullptr);
  const char* file_name_ct = utf8Str.c_str();

  FILE* fp;
  if (fopen_s(&fp, file_name_ct, "rb") != 0)
    error("Error: Failed to open the PNG file.");
  
  png_structp pngptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop pnginfo = png_create_info_struct(pngptr);
  
  png_init_io(pngptr, fp);
  png_read_info(pngptr, pnginfo);

  int width = png_get_image_width(pngptr, pnginfo);
  int height = png_get_image_height(pngptr, pnginfo);

  lua_State* L = luaL_newstate();
  luaL_openlibs(L);

  lua_newtable(L);
  int bmp_table_index = lua_gettop(L);
  
  png_bytep row = (png_bytep)malloc(png_get_rowbytes(pngptr, pnginfo));
  for (int y = 0; y < height; y++) {
    png_read_row(pngptr, row, NULL);
    for (int x = 0; x < width; x++) {
      png_bytep px = &(row[x * 3]);
      lua_newtable(L);
      int pixel_table_index = lua_gettop(L);
      lua_pushinteger(L, x);
      lua_setfield(L, pixel_table_index, "x");
      lua_pushinteger(L, y);
      lua_setfield(L, pixel_table_index, "y");
      lua_pushinteger(L, px[0]);
      lua_setfield(L, pixel_table_index, "r");
      lua_pushinteger(L, px[1]);
      lua_setfield(L, pixel_table_index, "g");
      lua_pushinteger(L, px[2]);
      lua_setfield(L, pixel_table_index, "b");
      lua_pushinteger(L, px[3]);
      lua_setfield(L, pixel_table_index, "a");
      lua_rawseti(L, -2, luaL_len(L, bmp_table_index) + 1);
    }
  }
  lua_setglobal(L, "bmp");
  print("bmp array was filled in Lua environment");

  free(row);
  png_destroy_read_struct(&pngptr, &pnginfo, NULL);
  fclose(fp);

  print("Ending image processing");

  std::filesystem::path cPath = std::filesystem::current_path();
  if (luaL_dofile(L, "settings.lua") != LUA_OK) {
    const char* errorMessage = lua_tostring(L, -1);
    std::cout << "Error while reading settings.lua: " << errorMessage << std::endl;
    lua_pop(L, 1);
  }
  else {
    print("Reading settings.lua");
    lua_getglobal(L, "engines");
    if (!lua_istable(L, -1)) {
      lua_close(L);
      return 1;
    }
    print("Executing engines");
    int n = luaL_len(L, -1);
    for (int i = 1; i <= n; ++i) {
      lua_rawgeti(L, -1, i);
      const char* fName = lua_tostring(L, -1);
      std::filesystem::path fPath = cPath / "output" / fName;
      try {
        std::filesystem::create_directory(fPath);
        std::filesystem::create_directory(fPath / "img");
      }
      catch (const std::exception& e) {
        std::cerr << "Failed to create folder: " << e.what() << std::endl;
      }
      std::string feName = fName + std::string(".lua");
      std::cout << "Executing " << feName << ":" << std::endl;
      int status = luaL_dofile(L, feName.c_str());
      if (status != LUA_OK) {
        const char* errorMessage = lua_tostring(L, -1);
        std::cout << "Error while executing " << feName << ": " << errorMessage << std::endl;
        lua_pop(L, 1);
      }
    }
  }

  lua_close(L);

  std::cout << "Done" << std::endl;
  system("pause");
  return 0;
}