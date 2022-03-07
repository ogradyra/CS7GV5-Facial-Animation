// Taken from https://www.keithlantz.net/2011/10/a-preliminary-wavefront-obj-loader-in-c/

#include "mesh_loader.h"
// Default Constructor
MeshLoader::MeshLoader() {}

// Constructor for models with textures
MeshLoader::MeshLoader(std::string filename) {
	std::cout << "Mesh: " << filename << std::endl;
	std::ifstream ifs(filename.c_str(), std::ifstream::in);
	std::string line, key;
	while (ifs.good() && !ifs.eof() && std::getline(ifs, line)) {
		key = "";
		std::stringstream stringstream(line);
		stringstream >> key >> std::ws;

		if (key == "v") { // vertex
			vertexData vertex; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				vertex.vertex.push_back(x);
			}
			vertices.push_back(vertex);
		}
		else if (key == "vt") { // texture coordinate
			vertexData vertex; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				vertex.vertex.push_back(x);
			}
			texCoords.push_back(vertex);
		}
		else if (key == "vn") { // normal
			vertexData vertex; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				vertex.vertex.push_back(x);
			}
			vertex.normalize();
			normals.push_back(vertex);
		}
		else if (key == "f") { // face
			face f; int v, t, n;
			while (!stringstream.eof()) {
				stringstream >> v >> std::ws;
				f.vertex.push_back(v - 1);
				if (stringstream.peek() == '/') {
					stringstream.get();
					if (stringstream.peek() == '/') {
						stringstream.get();
						stringstream >> n >> std::ws;
						f.normal.push_back(n - 1);
					}
					else {
						stringstream >> t >> std::ws;
						f.texture.push_back(t - 1);
						if (stringstream.peek() == '/') {
							stringstream.get();
							stringstream >> n >> std::ws;
							f.normal.push_back(n - 1);
						}
					}
				}
			}
			faces.push_back(f);
		}
		else {

		}
	}
	ifs.close();

	for (int f = 0; f < faces.size(); f++) {
		for (int v_i = 0; v_i < faces[f].vertex.size(); v_i++) {
			meshVertices.push_back(vertices[faces[f].vertex[v_i]].vertex[0]);
			meshVertices.push_back(vertices[faces[f].vertex[v_i]].vertex[1]);
			meshVertices.push_back(vertices[faces[f].vertex[v_i]].vertex[2]);
		}
		for (int vn_i = 0; vn_i < faces[vn_i].normal.size(); vn_i++) {
			meshNormals.push_back(normals[faces[f].normal[vn_i]].vertex[0]);
			meshNormals.push_back(normals[faces[f].normal[vn_i]].vertex[1]);
			meshNormals.push_back(normals[faces[f].normal[vn_i]].vertex[2]);
		}
		for (int vt_i = 0; vt_i < faces[vt_i].texture.size(); vt_i++) {
			meshTexCoords.push_back(texCoords[faces[f].texture[vt_i]].vertex[0]);
			meshTexCoords.push_back(texCoords[faces[f].texture[vt_i]].vertex[1]);
		}
	}
	numVertices = meshVertices.size() / 3;
}
