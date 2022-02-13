#ifndef _LinkedList_h_
#define _LinkedList_h_

#include"LinkedListNode.h"

namespace scutils
{
	template<typename T>
	class LinkedList
	{

	public:

		int Size() {
			return size;
		}

		bool Empty() {
			return (size == 0);
		}

		/**����ָ���ı���ҵ���Ӧ�Ľڵ�*/
		LinkedListNode<T> GetNode(int idx)
		{
			if (idx <= 0) return head;
			else if (idx >= size - 1) return tail;

			int i = 0;
			LinkedListNode<T> node = head;
			for (; node; node = node.next) {
				if (i == idx)
					return node;
				i++;
			}

			return nullptr;
		}

		void Clear()
		{
			head = tail = nullptr;
			size = 0;
		}

		LinkedListNode<T>* GetHeadNode()
		{
			return head;
		}

		LinkedListNode<T>* GetLastNode()
		{
			return tail;
		}

		/**�ϲ��б�����ձ��ϲ�list*/
		void Merge(LinkedList<T>& mergeList)
		{
			if (mergeList.Empty())
				return;

			if (head == nullptr)
			{
				head = mergeList.head;
				tail = mergeList.tail;
				size = mergeList.size;
				mergeList.Clear();
				return;
			}

			LinkedListNode<T>* node = mergeList.head;
			tail->next = mergeList.head;
			mergeList.head->prev = tail;
			tail = mergeList.tail;
			size += mergeList.size;
			mergeList.Clear();
		}

		// ������ĩβ����ڵ�
		LinkedListNode<T>* AddLast(T elem)
		{
			return AddBack(tail, elem);
		}

		// ������ĩβ����ڵ�
		LinkedListNode<T>* AddLast(LinkedListNode<T>* node)
		{
			return AddBack(tail, node);
		}


		// ������ĩβ����ڵ�
		LinkedListNode<T>* AddLast(LinkedListNode<T>* startNode, LinkedListNode<T>* endNode, int count)
		{
			return AddBack(tail, startNode, endNode, count);
		}


		// ������ͷ����ڵ�
		LinkedListNode<T>* AddFirst(T elem)
		{
			return AddBefore(head, elem);
		}

		// ������ͷ����ڵ�
		LinkedListNode<T>* AddFirst(LinkedListNode<T>* node)
		{
			return AddBefore(head, node);
		}

		// ������ͷ����ڵ�
		LinkedListNode<T>* AddFirst(LinkedListNode<T>* startNode, LinkedListNode<T>* endNode, int count)
		{
			return AddBefore(head, startNode, endNode, count);
		}

		LinkedListNode<T>* AddBack(LinkedListNode<T>* prev, T elem)
		{
			LinkedListNode<T>* node = new LinkedListNode<T>(elem);
			AddBack(prev, node);
			return node;
		}

		LinkedListNode<T>* AddBack(LinkedListNode<T>* prev, LinkedListNode<T>* node)
		{
			return AddBack(prev, node, node, 1);
		}

		LinkedListNode<T>* AddBack(LinkedListNode<T>* prev, LinkedList<T>& addList)
		{
			return AddBack(prev, addList.GetHeadNode(), addList.GetLastNode(), addList.Size());
		}

		LinkedListNode<T>* AddBack(
			LinkedListNode<T>* prev, LinkedListNode<T>* startNode, LinkedListNode<T>* endNode, int count)
		{
			if (startNode == nullptr || endNode == nullptr)
				return nullptr;

			if (head == nullptr)
			{
				head = startNode;
				tail = endNode;
				startNode->prev = nullptr;
				endNode->next = nullptr;
				size = count;
				return startNode;
			}

			if (prev == nullptr)
				prev = tail;

			LinkedListNode<T>* next = nullptr;
			if (prev != nullptr) {
				next = prev->next;
				prev->next = startNode;
			}

			endNode->next = next;
			startNode->prev = prev;

			if (next != nullptr)
				next->prev = endNode;

			if (prev == tail)
				tail = endNode;

			size += count;
			return startNode;
		}


		LinkedListNode<T>* AddBefore(LinkedListNode<T>* next, T elem)
		{
			LinkedListNode<T>* node = new LinkedListNode<T>(elem);
			AddBefore(next, node);
			return node;
		}

		LinkedListNode<T>* AddBefore(LinkedListNode<T>* next, LinkedListNode<T>* node)
		{
			return AddBefore(next, node, node, 1);
		}

		LinkedListNode<T>* AddBefore(LinkedListNode<T>* next, LinkedList<T>& addList)
		{
			return AddBefore(next, addList.GetHeadNode(), addList.GetLastNode(), addList.Size());
		}

		LinkedListNode<T>* AddBefore(
			LinkedListNode<T>* next, LinkedListNode<T>* startNode, LinkedListNode<T>* endNode, int count)
		{
			if (startNode == nullptr || endNode == nullptr)
				return nullptr;

			if (head == nullptr)
			{
				head = startNode;
				tail = endNode;
				startNode->prev = nullptr;
				endNode->next = nullptr;
				size = count;
				return startNode;
			}

			if (next == nullptr)
				next = head;

			LinkedListNode<T>* prev = nullptr;
			if (next != nullptr) {
				prev = next->prev;
				next->prev = endNode;
			}

			startNode->prev = prev;
			endNode->next = next;

			if (prev != nullptr)
				prev->next = startNode;

			if (next == head)
				head = startNode;

			size++;

			return startNode;
		}

		LinkedListNode<T>* Remove(LinkedListNode<T>* node)
		{
			return Remove(node, node, 1);
		}


		LinkedListNode<T>* Remove(LinkedListNode<T>* startNode, LinkedListNode<T>* endNode, int count)
		{
			if (startNode == nullptr || endNode == nullptr)
				return nullptr;

			LinkedListNode<T>* prev = startNode->prev;
			LinkedListNode<T>* next = endNode->next;

			if (prev != nullptr)
				prev->next = next;
			else
				head = next;

			if (next != nullptr)
				next->prev = prev;
			else
				tail = prev;

			size -= count;

			return next;
		}

		static void Sort(LinkedList<T>* list, Comparator<T>& cmp)
		{

			if (list == nullptr || list->Size() <= 1)
				return;

			SortCore(list->GetHeadNode(), list->GetLastNode(), cmp);
		}

	private:
		static void SortCore(LinkedListNode<T>* left, LinkedListNode<T>* right, Comparator<T>& cmp)
		{
			if (right == nullptr || left == nullptr || left == right->next) {
				return;
			}

			// base�д�Ż�׼��
			T base = left->elem;
			LinkedListNode<T>* i = left;
			LinkedListNode<T>* j = right;
			while (i != j) {
				// ˳�����Ҫ���ȴ��ұ߿�ʼ�����ң�ֱ���ҵ���baseֵС����
				while (cmp.compare(j->elem, base) != -1 && i != j) {
					j = j->prev;
				}

				// �ٴ������ұ��ң�ֱ���ҵ���baseֵ�����
				while (cmp.compare(i->elem, base) != 1 && i != j) {
					i = i->next;
				}

				// �����ѭ��������ʾ�ҵ���λ�û���(i>=j)�ˣ������������������е�λ��
				if (i != j) {
					T tmp = i->elem;
					i->elem = j->elem;
					j->elem = tmp;
				}
			}

			// ����׼���ŵ��м��λ�ã���׼����λ��
			left->elem = i->elem;
			i->elem = base;

			// �ݹ飬�������׼����������ִ�к�����ͬ���Ĳ���
			// i��������Ϊ������ȷ���õĻ�׼ֵ��λ�ã������ٴ���
			SortCore(left, i->prev, cmp);
			SortCore(i->next, right, cmp);
		}


	private:
		LinkedListNode<T>* head = nullptr;
		LinkedListNode<T>* tail = nullptr;
		int size = 0;

	};
}

#endif
