#pragma once

#include<climits>
#include<cstddef>

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

	MemoryPool()  noexcept;
	~MemoryPool() noexcept;
	// ���ص�ַ
	pointer address(reference x)const noexcept;
	const_pointer address(const_reference x)const noexcept;

	// �����������Ԫ������
	size_type max_size()const noexcept;
	// �ڴ�������/ɾ����Ԫ��
	template <class... Args> pointer newElement(Args&&... args);
	void deleteElement(pointer p);

	pointer allocate(size_type n = 1, const_pointer hint = 0);			// ���ڴ�������ڴ���Դ
	void deallocate(pointer p, size_type n = 1);						// ���ڴ���Դ���ظ��ڴ��			
	// �ڴ��Ԫ�صĹ���&����
	template <class U, class... Args>void construct(U* p, Args&&... args);
	template <class U> void destroy(U* p);

private:
	// �ڴ��Ϊ����������Blok�鴢�棬Slot_���Ԫ�ػ�nextָ��
	union Slot_
	{
		value_type element;
		Slot_*	   next;
	};
	typedef char*       data_pointer_;      
	typedef Slot_       slot_type_;			
	typedef Slot_*		slot_pointer_;

	slot_pointer_	BlockListHead_;			// �ڴ������ͷָ��
	slot_pointer_	SlotListHead_;			// Ԫ������ͷָ��
	slot_pointer_	lastSlot_;				// �ɴ��Ԫ�ص�����ָ��
	slot_pointer_	FreeSlotHead;			// Ԫ�ع�����ͷŵ����ڴ�����ͷָ��

	size_type padPointer(data_pointer_ p, size_type align) const noexcept;  // �����������ռ�
	void allocateBlock();													// ���뽫�ڴ��Ž��ڴ��

};
