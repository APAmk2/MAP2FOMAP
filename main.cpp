/*
MAP2FOMAP - A Fallout map TO Fonline text map converter
Copyright (C) 2025 APAMk2

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <filesystem>
#include <fstream>
#include <format>
#include <map>
#include <stdio.h>
#include <iostream>

#define TOML_EXCEPTIONS 0
#include "toml.hpp"
using namespace std::string_view_literals;

#include "FallShared.h"
#include "FallMap.h"

#if defined WIN32
#include "Windows.h"
#endif

const std::string FrmType[8] = { "items", "critters", "scenery", "walls", "tiles", "backgrnd", "intrface", "inven" };
const std::string ProtoTypeNames[6] = { "Items", "Critters", "Scenery", "Walls", "Tiles", "Misc" };

std::vector<std::string> tiles_names;

std::map<int, int> PidReplaceLst;
std::map<std::string, std::string> TileReplaceLst;

int GetRealPID(const int& pid)
{
	int result = pid;

	if (PidReplaceLst.contains(pid))
	{
		printf("%d = %d\n", pid, PidReplaceLst[pid]);
		result = PidReplaceLst[pid];
	}

	return result;
}

std::string GetRealTile(const std::string& tile)
{
	std::string result = tile;

	if (TileReplaceLst.contains(tile))
	{
		result = TileReplaceLst[tile];
	}

	return result;
}

std::filesystem::path GetProgramPath()
{
	std::string pathStr(256, 0);
#if defined WIN32
	int bytes = GetModuleFileName(NULL, &pathStr[0], 256);
#endif
	return std::filesystem::path(pathStr).parent_path();
}

bool LoadOptions()
{
	toml::parse_result result = toml::parse_file(GetProgramPath().string() + "\\config.toml");
	if (!result)
	{
		return false;
	}

	toml::table config = result.table();
	ConvOptions.FalloutPath = config["Config"]["FalloutPath"].value_or(""sv);
	ConvOptions.ConvListPath = config["Config"]["ConvertionList"].value_or(""sv);

	return true;
}

void LoadReplace()
{
	toml::parse_result result = toml::parse_file(GetProgramPath().string() + "\\" + ConvOptions.ConvListPath);
	if (!result)
	{
		return;
	}

	toml::table config = result.table();
	toml::table pids = *config["PIDS"].as_table();
	toml::table tiles = *config["TILES"].as_table();

	for (auto&& [key, val] : pids)
	{
		int pidToReplace = atoi(key.str().data());
		PidReplaceLst[pidToReplace] = val.as_integer()->get();
	}

	for (auto&& [key, val] : tiles)
	{
		TileReplaceLst[key.str().data()] = val.as_string()->get();
	}
}

void LoadTiles()
{
	std::string currString;
	std::filesystem::path tilesPath = ConvOptions.FalloutPath + "/data/art/tiles/TILES.LST";
	std::ifstream input(tilesPath);
	if (input.is_open())
	{
		while (getline(input, currString))
		{
			tiles_names.push_back(currString);
		}
	}
	input.close();
}

bool ReadFallMap(std::filesystem::path& filename, FallMap_t*& file)
{
	if (file != nullptr)
	{
		delete file;
		file = nullptr;
	}
	ByteReader* reader = new ByteReader;
	if (!reader->Reset(filename.string(), ByteReader::BigEndian)) return false;
	file = new FallMap_t(reader);
	file->Filename = filename.stem().string();
	reader->Close();
	delete reader;

	return true;
}

void GenerateHeader(std::ofstream& out, FallMap_t*& file)
{
	out << std::format("[{}]\n", "Header");
	out << std::format("{:20} {}\n", "Version", 4);
	out << std::format("{:20} {}\n", "MaxHexX", 200);

	int mapHexY = 200;
	switch (file->MapFlags & 0xE)
	{
	case 0x0c:
		mapHexY = 200;
		break;
	case 0x08:
		mapHexY = 400;
		break;
	case 0x00:
		mapHexY = 600;
		break;
	}

	out << std::format("{:20} {}\n", "MaxHexY", mapHexY);
	out << std::format("{:20} {}\n", "WorkHexX", 100);
	out << std::format("{:20} {}\n", "WorkHexY", 100);
	out << std::format("{:20} {}\n", "ScriptModule", "-");
	out << std::format("{:20} {}\n", "ScriptFunc", "-");
	out << std::format("{:20} {}\n", "NoLogOut", 0);
	out << std::format("{:20} {}\n", "Time", -1);
	out << std::format("{:20} {:<4} {:<4} {:<4} {:<4}\n", "DayTime", 5, 5, 5, 5);
	out << std::format("{:20} {:<3} {:<3} {:<3}\n", "DayColor0", 18, 18, 53);
	out << std::format("{:20} {:<3} {:<3} {:<3}\n", "DayColor1", 18, 18, 53);
	out << std::format("{:20} {:<3} {:<3} {:<3}\n", "DayColor2", 18, 18, 53);
	out << std::format("{:20} {:<3} {:<3} {:<3}\n", "DayColor3", 18, 18, 53);

	out << "\n";
}

void GenerateTiles(std::ofstream& out, FallMap_t*& file)
{
	out << std::format("[{}]\n", "Tiles");

	int tilesLen = 10000;

	switch (file->MapFlags & 0xE)
	{
	case 0x0c:
		tilesLen = 10000;
		break;
	case 0x08:
		tilesLen = 20000;
		break;
	case 0x00:
		tilesLen = 30000;
		break;
	}

	for (int i = 0; i < tilesLen; i++)
	{
		FallMapTile_t currentTile = file->Tiles[i];
		int X = (i % 100) * 2;
		int Y = (i / 100) * 2;

		if (currentTile.TileId > 1) out << std::format("{:10} {:<4} {:<4}           {}\n", "tile", X, Y, ("art\\tiles\\" + GetRealTile(tiles_names[currentTile.TileId])));
		if (currentTile.RoofId > 1) out << std::format("{:10} {:<4} {:<4}           {}\n", "roof", X, Y, ("art\\tiles\\" + GetRealTile(tiles_names[currentTile.RoofId])));
	}

	out << "\n";
}

void ExportFallMap(FallMap_t*& file)
{
	std::ofstream out;
	out.open(file->Filename + ".fomap");
	if (!out.is_open())
	{
		out.close();
		return;
	}

	GenerateHeader(out, file);
	GenerateTiles(out, file);

	out << std::format("[{}]\n", "Objects");

	//Entire
	out << std::format("{:20} {}\n", "MapObjType", 2);
	out << std::format("{:20} {}\n", "ProtoId", 3853);
	out << std::format("{:20} {}\n", "MapX", (file->DefChosPos % 200));
	out << std::format("{:20} {}\n", "MapY", ((file->DefChosPos / 200) + (200 * file->DefChosElv)));
	out << std::format("{:20} {}\n", "Scenery_ToDir", file->DefChosDir);

	for (size_t i = 0, len = file->TotalObjects; i < len; i++)
	{
		FallMapObject_t currObject = file->Objects[i];

		if (currObject.ObjectPos == -1) continue;

		if (currObject.PIDType == 0) // Items
		{
			out << std::format("{:20} {}\n", "MapObjType", 1);
			out << std::format("{:20} {}\n", "ProtoId", GetRealPID(currObject.PIDNum));

			int objX = (currObject.ObjectPos % 200);
			int objY = (currObject.ObjectPos / 200) + (200 * currObject.MapElev);
			out << std::format("{:20} {}\n", "MapX", objX);
			out << std::format("{:20} {}\n", "MapY", objY);
			if (currObject.WeapData != nullptr)
			{
				if (currObject.WeapData->AmmoPid > 0)
				{
					out << std::format("{:20} {}\n", "Item_AmmoPid", currObject.WeapData->AmmoPid);
				}
				if (currObject.WeapData->AmmoCount > 0)
				{
					out << std::format("{:20} {}\n", "Item_AmmoCount", currObject.WeapData->AmmoCount);
				}
			}
			if (currObject.KeyData != nullptr)
			{
				if (currObject.KeyData->KeyCode > 0)
				{
					out << std::format("{:20} {}\n", "Item_LockerDoorId", currObject.KeyData->KeyCode);
				}
			}

			if (currObject.DoorData != nullptr)
			{
				if (currObject.DoorData->WalkThru > 0)
				{
					out << std::format("{:20} {}\n", "Item_LockerCondition", currObject.DoorData->WalkThru);
				}
			}
		}

		if (currObject.PIDType == 1) // Critters
		{
			out << std::format("{:20} {}\n", "MapObjType", 0);
			out << std::format("{:20} {}\n", "ProtoId", currObject.PIDNum);

			int objX = (currObject.ObjectPos % 200);
			int objY = (currObject.ObjectPos / 200) + (200 * currObject.MapElev);
			out << std::format("{:20} {}\n", "MapX", objX);
			out << std::format("{:20} {}\n", "MapY", objY);
			out << std::format("{:20} {}\n", "Dir", currObject.Dir);
			out << std::format("{:20} {}\n", "Critter_Cond", 1);
			out << std::format("{:20} {}\n", "Critter_ParamIndex0", "ST_DIALOG_ID");
			out << std::format("{:20} {}\n", "Critter_ParamValue0", 0);
			out << std::format("{:20} {}\n", "Critter_ParamIndex1", "ST_AI_ID");
			out << std::format("{:20} {}\n", "Critter_ParamValue1", 1);
			out << std::format("{:20} {}\n", "Critter_ParamIndex2", "ST_BAG_ID");
			out << std::format("{:20} {}\n", "Critter_ParamValue2", 0);
			out << std::format("{:20} {}\n", "Critter_ParamIndex3", "ST_TEAM_ID");
			out << std::format("{:20} {}\n", "Critter_ParamValue3", currObject.CritData->GroupId);
			out << std::format("{:20} {}\n", "Critter_ParamIndex4", "ST_NPC_ROLE");
			out << std::format("{:20} {}\n", "Critter_ParamValue4", 0);
			out << std::format("{:20} {}\n", "Critter_ParamIndex5", "ST_REPLICATION_TIME");
			out << std::format("{:20} {}\n", "Critter_ParamValue5", 0);
		}

		if (currObject.PIDType == 2) // Scenery
		{
			out << std::format("{:20} {}\n", "MapObjType", 2);
			out << std::format("{:20} {}\n", "ProtoId", GetRealPID(2000 + currObject.PIDNum));

			int objX = (currObject.ObjectPos % 200);
			int objY = (currObject.ObjectPos / 200) + (200 * currObject.MapElev);
			out << std::format("{:20} {}\n", "MapX", objX);
			out << std::format("{:20} {}\n", "MapY", objY);
		}

		if (currObject.PIDType == 3) // walls
		{
			out << std::format("{:20} {}\n", "MapObjType", 2);
			out << std::format("{:20} {}\n", "ProtoId", GetRealPID(5000 + currObject.PIDNum));

			int objX = (currObject.ObjectPos % 200);
			int objY = (currObject.ObjectPos / 200) + (200 * currObject.MapElev);
			out << std::format("{:20} {}\n", "MapX", objX);
			out << std::format("{:20} {}\n", "MapY", objY);
		}

		if (currObject.PIDType == 5 && currObject.PIDNum != 12) // misc
		{
			out << std::format("{:20} {}\n", "MapObjType", 2);
			out << std::format("{:20} {}\n", "ProtoId", GetRealPID(4000 + currObject.PIDNum));

			int objX = (currObject.ObjectPos % 200);
			int objY = (currObject.ObjectPos / 200) + (200 * currObject.MapElev);
			out << std::format("{:20} {}\n", "MapX", objX);
			out << std::format("{:20} {}\n", "MapY", objY);
		}

		if (currObject.PIDType != 4) //Shared data
		{
			if (currObject.LightRadius > 0)
			{
				out << std::format("{:20} {}\n", "LightDistance", currObject.LightRadius);
			}
			if (currObject.LightIntense > 0)
			{
				out << std::format("{:20} {}\n", "LightIntensity", currObject.LightIntense);
			}

			out << "\n";
		}
	}

	out.close();
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("PATH TO MAP TO BE CONVERTED IS NOT SPECIFIED\n");
	}

	if (!LoadOptions())
	{
		printf("CANT INIT CONFIG!\n");
		getchar();
		return -1;
	}
	//getchar();
	if(ConvOptions.ConvListPath != "") LoadReplace();
	LoadTiles();
	FallMap_t* map = nullptr;
	std::filesystem::path mapPath = "";

	for (size_t i = 1; i < argc; i++)
	{
		mapPath = argv[i];
		ReadFallMap(mapPath, map);
		ExportFallMap(map);
	}
	
	return 0;
}
