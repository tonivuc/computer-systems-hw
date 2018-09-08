//
// Created by Toni on 08.09.2018.
//

#ifndef A1_MEMORY_MANAGER_LISTHEAD_H
#define A1_MEMORY_MANAGER_LISTHEAD_H


#include "BlockHeader.h"

class ListHead {

private:
	ListHead nextHead;
	BlockHeader firstHeader;
	unsigned int blockSize;

public:
	ListHead(const ListHead &nextHead, const BlockHeader &firstHeader, unsigned int blockSize);

	const ListHead &getNextHead() const;

	void setNextHead(const ListHead &nextHead);

	const BlockHeader &getFirstHeader() const;

	void setFirstHeader(const BlockHeader &firstHeader);

	unsigned int getBlockSize() const;

	void setBlockSize(unsigned int blockSize);
};


#endif //A1_MEMORY_MANAGER_LISTHEAD_H
