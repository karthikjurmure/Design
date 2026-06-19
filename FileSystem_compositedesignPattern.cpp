#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
class FileSystemNode {
public:
  virtual ~FileSystemNode() {}
  virtual void ls(int indent = 0) = 0;
  virtual void openAll(int indent = 0) = 0;
  virtual int getSize() = 0;
  virtual FileSystemNode *cd(const string &name) = 0;
  virtual string getName() = 0;
  virtual bool isFolder() = 0;
};
class File : public FileSystemNode {
private:
  string name;
  int size;

public:
  File(string n, int s) {
    this->name = n;
    this->size = s;
  }
  void ls(int indent = 0) override {
    cout << string(indent, ' ') << name << "\n";
  }
  void openAll(int indent = 0) override {
    cout << string(indent, ' ') << name << "\n";
  }
  int getSize() override { return size; }
  FileSystemNode *cd(const string &name) override { return nullptr; }
  string getName() override { return name; }
  bool isFolder() override { return false; }
};
class Folder : public FileSystemNode {
private:
  string name;
  vector<FileSystemNode *> children;

public:
  Folder(string name) { this->name = name; }
  ~Folder() {
    for (auto c : children)
      delete c;
  }
  void add(FileSystemNode *f) { children.push_back(f); }
  void ls(int indent = 0) override {
    for (auto child : children) {
      if (child->isFolder()) {
        cout << string(indent, ' ') << "+" << child->getName() << "\n";
      } else {
        cout << string(indent, ' ') << child->getName() << "\n";
      }
    }
  }
  void openAll(int indent = 0) override {
    cout << string(indent, ' ') << name << "\n";
    for (auto child : children) {
      child->openAll(indent + 4);
    }
  }
  int getSize() override {
    int total = 0;
    for (auto child : children) {
      total += child->getSize();
    }
    return total;
  }
  FileSystemNode *cd(const string &target) override {
    for (auto child : children) {
      if (child->isFolder() && child->getName() == target) {
        return child;
      }
    }
    return nullptr;
  }
  string getName() override { return name; }
  bool isFolder() override { return true; }
};
int main() {
  Folder *root = new Folder("root");
  root->add(new File("file1.txt", 1));
  root->add(new File("file2.txt", 1));
  Folder *docs = new Folder("docs");

  docs->add(new File("resume.pdf", 1));
  docs->add(new File("notes.txt", 1));
  root->add(docs);
  Folder *images = new Folder("images");
  images->add(new File("photo.jpg", 1));
  root->add(images);
  root->ls();
  docs->ls();
  root->openAll();
  FileSystemNode *cwd = root->cd("docs");
  if (cwd != nullptr) {
    cwd->ls();
  } else {
    cout << "\nfolder not found" << endl;
  }
  cout << "Total size: " << root->getSize();
  delete root;
  return 0;
}
