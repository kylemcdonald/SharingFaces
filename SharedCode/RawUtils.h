#pragma once

template <class T>
void saveRaw(fstream& file, const T& data) {
	file.write(reinterpret_cast<const char*>(&data), sizeof(T));
}

template <class T>
void saveRaw(fstream& file, const vector<T>& buffer) {
	file.write(reinterpret_cast<const char*>(&buffer[0]), sizeof(T) * buffer.size());
}

template <class T>
void loadRaw(fstream& file, T& data) {
	file.read(reinterpret_cast<char*>(&data), sizeof(T));
}

template <class T>
void loadRaw(fstream& file, vector<T>& buffer, int size) {
	buffer.resize(size);
	file.read(reinterpret_cast<char*>(&buffer[0]), sizeof(T) * size);
}
