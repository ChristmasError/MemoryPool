#pragma once 

#include"memory_pool_class.h"

template<typename T, size_t Blocksize>
inline typename MemoryPool<T, Blocksize>::size_type 
MemoryPool<T, Blocksize>::padPointer(data_pointer_ p, size_type align) const noexcept
{
	size_t result = reinterpret_cast<size_t>(p);
	return ((align - result) % align);
}
// ���캯��
template<typename T, size_t Blocksize>
MemoryPool<T, Blocksize>::MemoryPool() noexcept
{
	BlockListHead_ = 0;
	SlotListHead_  = 0;
	lastSlot_      = 0;
	FreeSlotHead   = 0;
} 

// ����������delete�ڴ�������е�block
template<typename T, size_t Blocksize>
MemoryPool<T, Blocksize>::~MemoryPool()
noexcept
{
	slot_pointer_ curr = BlockListHead_;
	while (curr != nullptr)//curr!=NULL
	{
		slot_pointer_ prev = curr->next;
		// ת��Ϊvoid* ֻ�ͷſռ䲻������������
		operator delete (reinterpret_cast<void*>(curr));
		curr = prev;
	}
}
// ���ص�ַ
template<typename T, size_t Blocksize >
inline typename MemoryPool<T, Blocksize>::pointer
MemoryPool<T, Blocksize>::address(reference x)
const noexcept
{
	return &x;
}
// ���ص�ַ const ���ذ汾
template<typename T, size_t Blocksize>
inline typename MemoryPool<T, Blocksize>::const_pointer
MemoryPool<T, Blocksize>::address(const_reference x)
const noexcept
{
	return &x;
}
// ����һ����е�Block�����ڴ��
template<typename T, size_t Blocksize>
void MemoryPool<T, Blocksize>::allocateBlock()
{
	// operator new��������һ��Blocksize��С���ڴ�
	data_pointer_ newBlock = reinterpret_cast<data_pointer_>(operator new(Blocksize));
	// ��Block����ͷ BlockListHead_
	reinterpret_cast<slot_pointer_>(newBlock)->next = BlockListHead_;
	BlockListHead_ = reinterpret_cast<slot_pointer_>(newBlock);
	// ����Ϊ�˶���Ӧ�ÿճ�����λ��
	data_pointer_ body = newBlock + sizeof(slot_pointer_);
	size_type bodyPadding = padPointer(body, sizeof(slot_type_));
	// currentslot_ Ϊ�� block ��ʼ�ĵط����� bodypadding �� char* �ռ�
	SlotListHead_ = reinterpret_cast<slot_pointer_>(body + bodyPadding);
	// �������һ���ܷ���slot_type_��λ��
	lastSlot_ = reinterpret_cast<slot_pointer_>(newBlock + Blocksize - sizeof(slot_type_) + 1);
}

// ����ָ�������Ԫ�������ڴ��ָ��
template<typename T, size_t Blocksize>
inline typename MemoryPool<T, Blocksize>::pointer
MemoryPool<T, Blocksize>::allocate(size_type n, const_pointer hint)
{
	// ���freeSlot_�ǿգ�����freeSlot_��ȥȡ�ڴ�
	if (FreeSlotHead != nullptr)
	{
		pointer pElement = reinterpret_cast<pointer>(FreeSlotHead);
		FreeSlotHead = FreeSlotHead->next;
		return pElement;
	}
	else
	{
		// Block���ڴ���������
		if (SlotListHead_ >= lastSlot_)
			allocateBlock();
		return reinterpret_cast<pointer>(SlotListHead_++);
	}
}
// ��Ԫ���ڴ�黹free�ڴ�����
template<typename T, size_t Blocksize>
inline void
MemoryPool<T, Blocksize>::deallocate(pointer p, size_type n)
{
	if (p != nullptr)
	{
		// ת����slot_pointer_ָ�룬nextָ��freeslots_����
		reinterpret_cast<slot_pointer_>(p)->next = FreeSlotHead;
		FreeSlotHead = reinterpret_cast<slot_pointer_>(p);
	}
}

// �������Ԫ�������� 
template<typename T, size_t Blocksize>
inline typename MemoryPool<T, Blocksize>::size_type
MemoryPool<T, Blocksize>::max_size()
const noexcept
{
	size_type maxBlocks = -1 / Blocksize;
	return ((Blocksize - sizeof(data_pointer_)) / sizeof(slot_type_)*maxBlocks);
}

// ���ѷ����ڴ��Ϲ������
template <typename T, size_t BlockSize>
template <class U, class... Args>
inline void
MemoryPool<T, BlockSize>::construct(U* p, Args&&... args)
{
	new (p) U(std::forward<Args>(args)...);
}

// ���ٶ���
template<typename T, size_t Blocksize>
template <class U>
inline void
MemoryPool<T, Blocksize>::destroy(U* p)
{
	p->~U();
	// ��������
}

// ������Ԫ��
template<typename T, size_t Blocksize>
template <class... Args>
inline typename  MemoryPool<T, Blocksize>::pointer
MemoryPool<T, Blocksize>::newElement(Args&&... args)
{
	// �����ڴ�
	pointer pElement = allocate();
	// ���ڴ��Ϲ������
	construct<value_type>(pElement, std::forward<Args>(args)...);
	return pElement;
}
// ɾ��Ԫ��
template<typename T, size_t Blocksize>
inline void
MemoryPool<T, Blocksize>::deleteElement(pointer p)
{
	if (p != nullptr)
	{
		p->~value_type();
		deallocate(p);
	}
}
