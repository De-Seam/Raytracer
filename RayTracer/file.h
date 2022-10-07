#pragma once
char* ReadFile(const char* filename);
void LoadObj(const char* filename, Vector<Triangle>* triangle, FColor color, float scale, int material);