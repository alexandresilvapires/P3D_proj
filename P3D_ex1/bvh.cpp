#include "rayAccelerator.h"
#include "macros.h"
#include <algorithm>
#include <stack>

using namespace std;

class ComparePrimitives {
public:
	bool operator() (Object* a, Object* b) {
		float ca = a->GetBoundingBox().centroid().getAxisValue(sort_dim);
		float cb = b->GetBoundingBox().centroid().getAxisValue(sort_dim);
		return ca < cb;
	}
	int sort_dim;
};

BVH::BVHNode::BVHNode(void) {}

void BVH::BVHNode::setAABB(AABB& bbox_) { this->bbox = bbox_; }

void BVH::BVHNode::makeLeaf(unsigned int index_, unsigned int n_objs_) {
	this->leaf = true;
	this->index = index_; 
	this->n_objs = n_objs_; 
}

void BVH::BVHNode::makeNode(unsigned int left_index_) {
	this->leaf = false;
	this->index = left_index_; 
			//this->n_objs = n_objs_; 
}


BVH::BVH(void) {}

int BVH::getNumObjects() { return objects.size(); }


void BVH::Build(vector<Object *> &objs) {

		
			BVHNode *root = new BVHNode();

			Vector min = Vector(FLT_MAX, FLT_MAX, FLT_MAX), max = Vector(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			AABB world_bbox = AABB(min, max);

			for (Object* obj : objs) {
				AABB bbox = obj->GetBoundingBox();
				world_bbox.extend(bbox);
				objects.push_back(obj);
			}
			world_bbox.min.x -= EPSILON; world_bbox.min.y -= EPSILON; world_bbox.min.z -= EPSILON;
			world_bbox.max.x += EPSILON; world_bbox.max.y += EPSILON; world_bbox.max.z += EPSILON;
			root->setAABB(world_bbox);
			nodes.push_back(root);
			build_recursive(0, objects.size(), root); // -> root node takes all the 
		}

int get_largest_dim(vector<Object*> objs) {
	float min_x = FLT_MAX;
	float max_x = FLT_MIN;
	float min_y = FLT_MAX;
	float max_y = FLT_MIN;
	float min_z = FLT_MAX;
	float max_z = FLT_MIN;

	for (Object* o : objs) {
		Vector c = o.GetBoundingBox().centroid();
		if (min_x > c.x) min_x = c.x;
		if (max_x < c.x) max_x = c.x;
		if (min_y > c.y) min_y = c.y;
		if (max_y < c.y) max_y = c.y;
		if (min_z > c.z) min_z = c.z;
		if (max_z < c.z) max_z = c.z;
	}
	float x_interval = max_x - min_x;
	float y_interval = max_y - min_y;
	float z_interval = max_z - min_z;

	if (x_interval >= y_interval && x_interval >= z_interval) return 0;
	else if (y_interval >= x_interval && y_interval >= z_interval) return 1;
	else return 2;
}

void BVH::build_recursive(int left_index, int right_index, BVHNode *node) {
		//right_index, left_index and split_index refer to the indices in the objects vector
	   // do not confuse with left_nodde_index and right_node_index which refer to indices in the nodes vector. 
	    // node.index can have a index of objects vector or a index of nodes vector

	if (right_index - left_index <= 2) {
		BVHNode* n = new BVHNode();
		n->makeLeaf(left_index, right_index - left_index + 1);

	}
	else {
		// Find shortest dimension in AABB and use it to compare objects
		ComparePrimitives cmp = ComparePrimitives();

		// find shortest dim
		float size_x = node->getAABB().max.x - node->getAABB().min.x;
		float size_y = node->getAABB().max.y - node->getAABB().min.y;
		float size_z = node->getAABB().max.z - node->getAABB().min.z;

		if (size_y >= size_x && size_y >= size_z) cmp.sort_dim = 1;
		else if (size_z >= size_x && size_z >= size_y) cmp.sort_dim = 1;

		//std::sort(getObjs().begin() + left_index, getObjs().begin() + right_index, cmp); // TODO access objects

		//TODO find split index
		int split_index = (left_index + right_index) / 2;


		// Calculate bounding boxes of left and right sides
		AABB left_bb = getObjs()[left_index]->GetBoundingBox();
		for (int i = left_index+1; i < split_index; i++) {
			left_bb.extend(getObjs()[i]->GetBoundingBox());
		}

		AABB right_bb = getObjs()[split_index]->GetBoundingBox();
		for (int i = split_index; i < right_index; i++) {
			right_bb.extend(getObjs()[i]->GetBoundingBox());
		}

		// Create two new nodes, leftNode and rightNode and assign bounding boxes
		BVHNode* left_node = new BVHNode();
		left_node->setAABB(left_bb);

		BVHNode* right_node = new BVHNode();
		right_node->setAABB(right_bb);

		// Initiate current node as an interior node with leftNode and rightNode as children:
		node->makeNode(nodes.size());
		node->setIndex(left_node->getIndex());

		// Push back leftNode and rightNode into nodes vector
		nodes.push_back(left_node);
		nodes.push_back(right_node);

		build_recursive(left_index, split_index, left_node);
		build_recursive(split_index, right_index, right_node);
	}
			
		
	}

bool BVH::Traverse(Ray& ray, Object** hit_obj, Vector& hit_point) {
			float tmp1, tmp2;
			float tmin = FLT_MAX;  //contains the closest primitive intersection
			bool hit = false;

			BVHNode* currentNode = nodes[0];

			bool hit_world = currentNode->getAABB().intercepts(ray, tmin);

			if (hit_world == false) return false;

			while (true) {
				if (!currentNode->isLeaf()) {
					// Case for not in leaf
					BVHNode* left = nodes[currentNode->getIndex()];
					BVHNode* right = nodes[currentNode->getIndex() + 1];

					bool hit_left = left->getAABB().intercepts(ray, tmp1);
					bool hit_right = right->getAABB().intercepts(ray, tmp2);

					if (hit_left && hit_right) {
						// Both hit: furthest one goes to the stack, current node is now the closest
						if (tmp1 < tmp2) {
							hit_stack.push(StackItem(right, tmp2));
							currentNode = left;
						}
						else {
							hit_stack.push(StackItem(left, tmp1));
							currentNode = right;
						}
					}
					// If only one hit, that one becomes the current node
					else if (hit_left) {
						currentNode = left;
					}
					else if (hit_right) {
						currentNode = right;
					}
				}
				else {
					// Case for leaf
					for (int i = 0; i <= currentNode->getNObjs(); i++) {
						Object* o = getObjs()[currentNode->getIndex() + i];

						bool has_hit = o->intercepts(ray, tmp1);

						if (has_hit) {
							if (tmp1 < tmin) {
								tmin = tmp1;
								hit_obj = &o;
								hit = true; // Se calhar está errado
							}
						}
					}
				}
				while (!hit_stack.empty()) {
					StackItem st = hit_stack.top();
					hit_stack.pop();
					if (st.t < tmin) {
						currentNode = st.ptr;
					}
				}
				if (hit_stack.empty()) {
					hit_point = ray.origin + ray.direction * tmin;
					return hit == true;
				}
			}

			// Never reached
			return(false);
	}

bool BVH::Traverse(Ray& ray) {  //shadow ray with length

			// TODO check what to do with this
			double length = ray.direction.length(); //distance between light and intersection point
			ray.direction.normalize();

			float tmp1, tmp2;
			float tmin = FLT_MAX;  //contains the closest primitive intersection
			bool hit = false;

			BVHNode* currentNode = nodes[0];

			bool hit_world = currentNode->getAABB().intercepts(ray, tmin);

			if (hit_world == false) return false;

			while (true) {
				if (!currentNode->isLeaf()) {
					// Case for not in leaf
					BVHNode* left = nodes[currentNode->getIndex()];
					BVHNode* right = nodes[currentNode->getIndex() + 1];

					bool hit_left = left->getAABB().intercepts(ray, tmp1);
					bool hit_right = right->getAABB().intercepts(ray, tmp2);

					if (hit_left && hit_right) {
						// Both hit: furthest one goes to the stack, current node is now the closest
						hit_stack.push(StackItem(left, tmp1));
						currentNode = right;
					}
					// If only one hit, that one becomes the current node
					else if (hit_left) {
						currentNode = left;
					}
					else if (hit_right) {
						currentNode = right;
					}
				}
				else {
					// Case for leaf
					for (int i = 0; i <= currentNode->getNObjs(); i++) {
						Object* o = getObjs()[currentNode->getIndex()+i];

						bool has_hit = o->intercepts(ray, tmp1);

						if (has_hit) {
							return true;
						}
					}
				}
				if (hit_stack.empty()) {
					return false;
				}
				else {
					StackItem st = hit_stack.top();
					hit_stack.pop();
					currentNode = st.ptr;
				}
			}

			// Never reached
			return(false);
	}		
