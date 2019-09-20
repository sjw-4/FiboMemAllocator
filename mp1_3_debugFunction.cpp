bool MyAllocator::debugSegFault(){		//input is fls, lastP (which is initP + _size)
	for(int i = 0; i < fls.size(); i++){
		SegmentHeader * temp = new SegmentHeader;
		temp = fls.at(i)->head;
		while(temp != nullptr){
			if((char*)temp + temp->length > lastP) {
				cout << "Seg fault detected at fls index: " << i << endl;
				dumpFls();
			}
		}
	}
}