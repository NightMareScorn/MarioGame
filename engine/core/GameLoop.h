#pragma once
#include <windows.h>
#include <string>

void Update(float dt);
void Render();
int Run(std::string level = "content/levels/level_1_1.csv");
