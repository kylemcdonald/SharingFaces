#pragma once

template <class T>
void saveRaw(ofstream& file, const T& data) {
	file.write(reinterpret_cast<const char*>(&data), sizeof(T));
}

template <class T>
void saveRaw(ofstream& file, const vector<T>& buffer) {
	file.write(reinterpret_cast<const char*>(&buffer[0]), sizeof(T) * buffer.size());
}

template <class T>
void loadRaw(ifstream& file, T& data) {
	file.read(reinterpret_cast<char*>(&data), sizeof(T));
}

template <class T>
void loadRaw(ifstream& file, vector<T>& buffer, int size) {
	buffer.resize(size);
	file.read(reinterpret_cast<char*>(&buffer[0]), sizeof(T) * size);
}
