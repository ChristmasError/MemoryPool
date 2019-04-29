#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H
#pragma once
#include<climits>
#include<cstddef>
//#include<stddef.h>
template<typename T, size_t Blocksize = 4096>
class MemoryPool
{
public:
	typedef       T				 value_type;
	typedef       T*		     pointer;
	typedef       T&			 reference;
	typedef const T*			 const_pointer;
	typedef const T&			 const_reference;
	typedef       size_t		 size_type;
	typedef		  ptrdiff_t		 difference_type;
	// ptrdiff_t: ������ָ������Ľ�����޷�����������
	// size_t : ��sizeof�������Ľṹ���޷�������

	template<typename U>struct rebind {
		typedef MemoryPool<U>other;
	};
	// ����&��������*
	MemoryPool() noexcept;
	MemoryPool(const MemoryPool&memorypool)noexcept;
	template<class U>MemoryPool(const MemoryPool<U>&memorypool)noexcept;
	~MemoryPool()noexcept;
	// ���ص�ַ
	pointer address(reference x)const noexcept;
	const_pointer address(const_reference x)const noexcept;

	// �����������Ԫ������
	size_type max_size()const noexcept;
	// �Դ� �����ڴ� �� �ͷ��ڴ� �Ĺ���������
	template <class... Args> pointer newElement(Args&&... args);
	void deleteElement(pointer p);

	pointer allocate(size_type n = 1, const_pointer hint = 0);			// ����һ���ڴ�ռ�
	void deallocate(pointer p, size_type n = 1);						// ����һ���ڴ�ռ�			
	// �ڴ��Ԫ�صĹ���&����
	template <class U, class... Args>void construct(U* p, Args&&... args);
	template <class U> void destroy(U* p);

private:
	// �ڴ��Ϊ����������Blok�鴢�棬Slot_���Ԫ�ػ�nextָ��
	union Slot_
	{
		value_type element;
		Slot_* next;
	};
	typedef char*       data_pointer_;      // char*ָ���ڴ���׵�ַ
	typedef Slot_       slot_type_;			// Ԫ������
	typedef Slot_*		slot_pointer_;

	slot_pointer_	currentBlock_;			// �ڴ������ͷָ��
	slot_pointer_	currentSlot_;			// Ԫ������ͷָ��
	slot_pointer_	lastSlot_;				// �ɴ��Ԫ�ص�����ָ��
	slot_pointer_	freeSlots_;				// Ԫ�ع�����ͷŵ����ڴ�����ͷָ��

	size_type padPointer(data_pointer_ p, size_type align) const noexcept;  // �����������ռ�
	void allocateBlock();													// ���뽫�ڴ��Ž��ڴ��

};


#endif // !MEMORY_POOL_H

