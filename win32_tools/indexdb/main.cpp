#include <iostream>
#include <fstream>      // filebuf
#include <format>
#include <string>
#include <filesystem>
#include <string_view>
#include <map>
#include "cargs.h"

#include "cargs.h"
#include "rapidcsv.h"

#include "../../libs/atomdb/db_priv.h"

#ifdef assert
#undef asset
#endif

#include <cassert>

using namespace std::filesystem;
using namespace std;

static void print_help()
  {
  cerr << "Usage: indexdb [Options] <output path>" << endl
    << " Options are one of:" << endl
    << "    -h      Print help" << endl
    << "    -s <path>  Index database metadata given by <path> (required)" << endl
    << "    -t <type>  set the index type (required)" << endl
    << "    -d <name>  Set the name of the indexed container (required)" << endl
    << "    -m <name>  Set the name of the field that is indexed (required)" << endl
    << "    -n <name>  set the name (required)" << endl
    << "example: indexdb -s /Projects/map_data/cities.csv -n \"Cities by name\" /Projects/map_data/cities-by-name.db" << endl;
  }

/* The metadata database is a CSV file in the form:
* entity_id,field-name,field-value
* <entity_id>,<field_name>,<value>
* 
* 
*/

struct export_type_t {
  const char* name;
  index_data_type type;
  };

static const export_type_t export_options[] =
  {
  { "string", idt_string },
  { "int32", idt_int32 },
  { "uint32", idt_uint32 },
  { "fixed", idt_fixed },
  { "float", idt_float },
  { 0, (index_data_type) 0 },
  };


static void init_node(uint32_t page, index_node_t* node)
  {
  memset(node, 0, sizeof(index_node_t));
  node->page_index = page;
  }

static vector<char> packed_strings;
static vector<uint32_t> object_lists;
typedef vector<index_node_t> nodes_t;
static nodes_t nodes;

static int compare_key(const index_key_t& k1, const index_key_t& k2)
  {
  const char *str1 = packed_strings.data() + k1.offset - sizeof(index_container_header_t);
  const char *str2 = packed_strings.data() + k2.offset - sizeof(index_container_header_t);

  return strcmp(str1, str2);
  }

static index_node_t *add_node(uint32_t parent, const index_key_t &key)
  {
  uint32_t index = nodes.size();
  index_node_t new_node;
  memset(&new_node, 0, sizeof(index_node_t));
  new_node.page_index = index;
  new_node.parent = parent;
  new_node.num_entries = 1;
  new_node.keys[0] = key;

  nodes.push_back(new_node);

  return &nodes.at(index);
  }

static index_node_t* create_node(index_node_t *parent_node, size_t start_key, size_t num_keys)
  {
  index_node_t* result = add_node(parent_node == nullptr ? 0 : parent_node->page_index, parent_node->keys[start_key]);
  num_keys--;
  start_key++;
  while (num_keys-- > 0)
    result->keys[result->num_entries++] = parent_node->keys[start_key++];

  return result;
  }

static pair<index_node_t *, uint32_t> add_value(index_node_t* node, const index_key_t &key);

static index_node_t* split_node(index_node_t *node)
  {
  assert(node->child == 0);           
  assert(node->num_entries == 31);

  index_node_t *right_node = create_node(node, 16, 15);
  node->keys[15].child = right_node->page_index;

  // make the median the split
  if(node->parent == 0)
    {
    index_node_t *parent = add_node(0, node->keys[15]);
    parent->child = node->page_index;     // assign as the left node
    }
  else
    {
    pair<index_node_t *, uint32_t> parent_key = add_value(&nodes[node->parent], node->keys[15]);
    if(parent_key.second == 0)
      parent_key.first->child = node->page_index;     // is first key in the parent
    else
      parent_key.first->keys[parent_key.second-1].child = node->page_index;
    }

  // reset the keys that were moved
  for(size_t i = 15; i < 31; i++)
    memset(&node->keys[i], 0, sizeof(index_node_t));

  node->num_entries = 15;

  return node;
  }

static pair<index_node_t*, uint32_t> add_value(index_node_t* node, const index_key_t &key)
  {
  uint32_t pos;
  if (compare_key(key, node->keys[0]) < 0)
    pos = 0;
  else
    {
    // scan backwards
    for (pos = node->num_entries; compare_key(key, node->keys[pos - 1]) < 0 && pos > 0; pos--);
    pos--;
    if (compare_key(key, node->keys[pos]) == 0)
      {
      cerr << "Fatal error: Duplicate key found" << endl;
      exit(-1);
      }
    }

  if (pos < node->num_entries && node->num_entries < 31)
    {
    // insert the key
    for(uint32_t dst = node->num_entries; dst > pos; dst--)
      node->keys[dst] = node->keys[dst-1];

    node->keys[pos] = key;
    }
  else
    {
    // split the node create a pair of nodes
    node = split_node(node);
    node = add_value(node, key).first;
    }

  return make_pair(node, pos);
  }

static void add_key(const pair<uint32_t, uint32_t>& key_value, uint32_t num_oids)
  {
  // create a key to insert
  index_key_t key;

  key.offset = key_value.first;
  key.value = key_value.second;
  key.length = num_oids == 1 ? 0 : num_oids;
  key.child = 0;        // no children yet

  if (nodes.size() == 0)
    {
    add_node(0, key);
    return;
    }

  // insert the node at the root node
  for (nodes_t::iterator it = nodes.begin(); it != nodes.end(); it++)
    {
    if(it->parent == 0)
      add_value(it.operator->(), key);
    }
  }

/*
// Inserting a key on a B-tree in C++

#include <iostream>
using namespace std;

class Node {
  int *keys;
  int t;
  Node **C;
  int n;
  bool leaf;

   public:
  Node(int _t, bool _leaf);

  void insertNonFull(int k);
  void splitChild(int i, Node *y);
  void traverse();

  friend class BTree;
};

class BTree {
  Node *root;
  int t;

   public:
  BTree(int _t) {
    root = NULL;
    t = _t;
  }

  void traverse() {
    if (root != NULL)
      root->traverse();
  }

  void insert(int k);
};

Node::Node(int t1, bool leaf1) {
  t = t1;
  leaf = leaf1;

  keys = new int[2 * t - 1];
  C = new Node *[2 * t];

  n = 0;
}

// Traverse the nodes
void Node::traverse() {
  int i;
  for (i = 0; i < n; i++) {
    if (leaf == false)
      C[i]->traverse();
    cout << " " << keys[i];
  }

  if (leaf == false)
    C[i]->traverse();
}

// Insert the node
void BTree::insert(int k) {
  if (root == NULL) {
    root = new Node(t, true);
    root->keys[0] = k;
    root->n = 1;
  } else {
    if (root->n == 2 * t - 1) {
      Node *s = new Node(t, false);

      s->C[0] = root;

      s->splitChild(0, root);

      int i = 0;
      if (s->keys[0] < k)
        i++;
      s->C[i]->insertNonFull(k);

      root = s;
    } else
      root->insertNonFull(k);
  }
}

// Insert non full condition
void Node::insertNonFull(int k) {
  int i = n - 1;

  if (leaf == true) {
    while (i >= 0 && keys[i] > k) {
      keys[i + 1] = keys[i];
      i--;
    }

    keys[i + 1] = k;
    n = n + 1;
  } else {
    while (i >= 0 && keys[i] > k)
      i--;

    if (C[i + 1]->n == 2 * t - 1) {
      splitChild(i + 1, C[i + 1]);

      if (keys[i + 1] < k)
        i++;
    }
    C[i + 1]->insertNonFull(k);
  }
}

// split the child
void Node::splitChild(int i, Node *y) {
  Node *z = new Node(y->t, y->leaf);
  z->n = t - 1;

  for (int j = 0; j < t - 1; j++)
    z->keys[j] = y->keys[j + t];

  if (y->leaf == false) {
    for (int j = 0; j < t; j++)
      z->C[j] = y->C[j + t];
  }

  y->n = t - 1;
  for (int j = n; j >= i + 1; j--)
    C[j + 1] = C[j];

  C[i + 1] = z;

  for (int j = n - 1; j >= i; j--)
    keys[j + 1] = keys[j];

  keys[i] = y->keys[t - 1];
  n = n + 1;
}

int main() {
  BTree t(3);
  t.insert(8);
  t.insert(9);
  t.insert(10);
  t.insert(11);
  t.insert(15);
  t.insert(16);
  t.insert(17);
  t.insert(18);
  t.insert(20);
  t.insert(23);

  cout << "The B-tree is: ";
  t.traverse();
}*/

static bool has_option(cargs_t& args, const char* opt)
  {
  return args.options().find(opt) != args.options().end();
  }

int main(int argc, const char** argv)
  {
  cargs_t args;

  args.parse(argc, argv);

  if (args.arguments().size() < 1)
    {
    print_help();
    exit(-1);
    }

  if (has_option(args, "h") ||
    !has_option(args, "s") ||
    !has_option(args, "d") ||
    !has_option(args, "m") ||
    !has_option(args, "n") ||
    args.arguments().empty())
    {
    print_help();
    exit(-1);
    }

  string source_filename = args.options()["s"];
  string field_name = args.options()["m"];

  string db_type_str = args.options()["t"];

  index_data_type type = (index_data_type) 0;
  for (const export_type_t* export_str = export_options; export_str->name != 0; export_str++)
    {
    if (db_type_str == export_str->name)
      {
      type = export_str->type;
      break;
      }
    }

  if (type == (index_data_type)0)
    {
    cerr << "The index type " << db_type_str << " is not supported" << endl;
    exit(-1);
    }

  rapidcsv::Document doc(source_filename);

  typedef vector<uint32_t> oids_t;

  typedef map<string, oids_t > keys_t;
  keys_t keys;

  for (size_t i = 0; i < doc.GetRowCount(); i++)
    {
    vector<string> fields = doc.GetRow<string>(i);

    if (fields.size() != 3)
      {
      cerr << "Row " << i << " does not contain a tuple of 3" << endl;
      continue;
      }

    if (fields[2].size() == 0)
      {
      cerr << "Row " << i << " has a null value type, ignoring row" << endl;
      continue;
      }

    string entity_id = fields[0];

    if (entity_id.size() == 0)
      {
      cerr << "Row " << i << " has a null oid, ignore row" << endl;
      continue;
      }

    uint32_t oid = stoul(entity_id);

    string key_value =  fields[2];

    transform(key_value.begin(), key_value.end(), key_value.begin(),
      [](unsigned char c) { return std::tolower(c); });

    if (fields[1] == field_name)
      {
      // look up the index value
      keys_t::iterator it = keys.find(key_value);

      if(it == keys.end())
        it = keys.insert({ key_value, oids_t() }).first;

      // check that the oid is not in there

      oids_t::iterator obj_it = std::find(it->second.begin(), it->second.end(), oid);
      if(obj_it == it->second.end())
        it->second.push_back(oid);      // save the object that contains the index value
      }
    }

  // at this point the map contains all keys and all oid's associated with the key
  // value.
  index_container_header_t hdr;
  memset(&hdr, 0, sizeof(index_container_header_t));

  // extract the map into a sorted list
  // the second value holds the index into the string table that is stored
  typedef vector<pair<string, pair<pair<uint32_t, uint32_t>, oids_t > > > sorted_keys_t;
  sorted_keys_t sorted_keys;

  uint32_t string_offset = sizeof(index_container_header_t);
  hdr.base.string_table = string_offset;
  hdr.data_type = type;       // store the index type.

  // prepare the vector, allocate the string index and oid list.
  // note if the 
  for (keys_t::const_iterator it = keys.begin(); it != keys.end(); it++)
    {
    // pack the string into the database
    for(size_t s = 0; s <= it->first.size(); s++)
      packed_strings.push_back(it->first.c_str()[s]);

    sorted_keys.push_back({ it->first, { { string_offset, 0 }, it->second } });
    // add the string that was stored.
    string_offset += it->first.size()+1;
    }

  hdr.base.string_table_length = string_offset - sizeof(index_container_header_t);

  sort(sorted_keys.begin(), sorted_keys.end(), 
    [](pair<string, pair<pair<uint32_t, uint32_t>, oids_t > > &a, pair<string, pair<pair<uint32_t, uint32_t>, oids_t > >  &b) throw()
      { return a.first < b.first; });

  // the strings are built, now calculate the oid list offsets (second pair of pair<uint32_t, uint32_t>
  uint32_t oid_base = string_offset;
  for (sorted_keys_t::iterator it = sorted_keys.begin(); it != sorted_keys.end(); it++)
    {
    if (it->second.second.size() > 1)
      {
      it->second.first.second = oid_base;
      // more than 1 oid in the list
      for(size_t i = 0; i < it->second.second.size(); i++)
        object_lists.push_back(it->second.second[i]);

      // addjust the base for the number of 4 byte oid's
      oid_base += it->second.second.size() << 2;
      }
    }

  // create the output database.
  string output_path = args.arguments()[0];

  cout << "Create container " << output_path << endl;
  // write the database
  ofstream os(output_path, ios_base::binary | ios_base::trunc | ios_base::out);

  // write the header
  os.write(reinterpret_cast<const char*>(&hdr), sizeof(geospatial_container_header_t));

  if(type == idt_string)
    // write the strings
    os.write(packed_strings.data(), packed_strings.size());

  // write the lists of OID's
  os.write(reinterpret_cast<const char *>(object_lists.data()), object_lists.size() << 2);

  // build the b+tree
  // since the sorted nodes are in sort order, the nodes can be created easily
  for(sorted_keys_t::iterator it = sorted_keys.begin(); it != sorted_keys.end(); it++)
    add_key(it->second.first, it->second.second.size());

  // the nodes can now be written
  for (nodes_t::const_iterator it = nodes.begin(); it != nodes.end(); it++)
    os.write(reinterpret_cast<const char *>(it.operator->()), sizeof(index_node_t));

  os.flush();
  os.close();

  cout << "Container created" << endl;

  return 0;
  }
