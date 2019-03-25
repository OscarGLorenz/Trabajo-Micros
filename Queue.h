/***********************************************************************
 * FILENAME :        Queue.h
 *
 * DESCRIPTION :
 *       A dinamic queue template class for C++11
 * 		Insert at back, delete at front
 * 		No std namespace
 * 		Compatible with Arduino
 *
 * AUTHOR :    Óscar García       START DATE :    31 jul 16
 ***********************************************************************/

#ifndef QUEUE_H_
#define QUEUE_H_

namespace dyn {
template<class T> class Node_Queue {
public:
	Node_Queue(T param) {
		object = param;
		next = nullptr;
	}
	T object;
	Node_Queue<T> * next;
};

template<class T> class Queue_Iterator {
	/** Iterator is recomended using "while (Iterator<T>.hasNext()) {T = Iterator<T>.next();}" */
public:
	Queue_Iterator(Node_Queue<T> * first_node) {
		node = first_node;
		first = true;
	}

	bool hasNext() {
		/** Returns true if there are more elements after this */
		return (node->next != nullptr);
	}

	T next() {
		/** Returns next element, it starts before first element */
		if (first) {
			first = false;
			return node->object;
		} else {
			Node_Queue<T> * next = node->next;
			node = node->next;
			return next->object;
		}
	}
private:
	Node_Queue<T> * node;
	bool first;
};

template<class T> class Queue {
public:
	Queue() {
		/** Creates an empty Queue */
		first = nullptr;
		last = nullptr;
		lenght = 0;
	}

	Queue(T * array, int size) :
			Queue() {
		/** Creates a queue with given array */
		for (int i = 0; i < size; i++) {
			pushBack(array[i]);
		}
	}

	void set(T object, int index) {
		/** Overwrite the given value in the given index of the Queue */
		Queue_Iterator<T> itr = getIterator();

		for (int i = 0; itr.hasNext(); i++) {
			if (index == i) {
				&itr.next() = new Node_Queue<T>(object);
			} else {
				itr.next();
			}
		}
	}

	void pushBack(T object) {
		/** Appends at back given value */
		if (first == nullptr) {
			first = new Node_Queue<T>(object);
			last = first;
		} else {
			last->next = new Node_Queue<T>(object);
			last = last->next;
		}
		lenght++;
	}

	void pushBack(T * array, int size) {
		/** Appends the give array to the queue */
		for (int i = 0; i < size; i++) {
			pushBack(array[i]);
		}
	}

	void popFront() {
		/** Deletes first object */
		if (first != nullptr) {
			if (last != first) {
				Node_Queue<T> * aux = first->next;
				delete first;
				first = aux;
			} else {
				delete first;
				first = nullptr;
				last = nullptr;
			}
			lenght--;
		}
	}

	T * toArray(int * size = nullptr) {
		/** Returns the queue in an array, optional reference parameter returns lenght of
		 * the array */
		T * array = new T[lenght];

		if (size != nullptr)
			*size = lenght;

		Queue_Iterator<T> itr = getIterator();

		for (int i = 0; itr.hasNext(); i++) {
			array[i] = itr.next();
		}

		return array;
	}

	T getInt(int index) {
		/** Returns the object at the given index */
		Queue_Iterator<T> itr = getIterator();

		for (int i = 0; itr.hasNext(); i++) {
			if (index == i) {
				return itr.next();
			} else {
				itr.next();
			}
		}

		return T();
	}

	Queue_Iterator<T> getIterator() {
		/** Returns an iterator of this object */
		return Queue_Iterator<T>(first);
	}

	bool has(T object, int * index = nullptr) {
		/** Returns true if the array contains the given object, optional parameter returns first occurence */
		Queue_Iterator<T> itr = getIterator();

		for (int i = 0; itr.hasNext(); i++) {
			T obj = itr.next();
			if (obj == object) {
				if (index != nullptr)
					*index = i;
				return true;
			}
		}
		return false;
	}

	int size() {
		/** Returns actual size of the Queue */
		return lenght;
	}

	bool isEmpty() {
		/** Returns true if empty */
		return lenght == 0;
	}

	~Queue() {
		while (lenght != 0) {
			popFront();
		}
	}

private:
	Node_Queue<T> * first;
	Node_Queue<T> * last;

	int lenght;
};

}

#endif

