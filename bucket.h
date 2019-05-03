#ifndef __BUCKET_H__
#define __BUCKET_H__

//#include <typeinfo>

/*NOTE(): Bucket Example:

	bucket_list buckets = {};
	InitBucket(&buckets);

	s32 *one = S32("one\n");
	s32 *two = S32("two\n");
	s32 *three = S32("three\n");
	s32 *four = S32("three3\n");
	s32 *five= S32("three4\n");
	s32 *six= S32("three5\n");

	AddToBucket(&buckets,"one", (i8*)  one);
	AddToBucket(&buckets,"two", (i8*) two); 
	AddToBucket(&buckets,"three", (i8*) three); 
	AddToBucket(&buckets,"four", (i8*) four); 
	AddToBucket(&buckets,"five", (i8*) five); 
	AddToBucket(&buckets,"six", (i8*) six); 
	FreeAllBuckets(&buckets);
*/


typedef i32 (*HASHFUNCTION)(s32* key, i8* ptrToValue);
#define AddS32ToBucket(chunk,key,value) do {  s32 *tempKey = S32(key);s32 *tempChar = S32((char*)value); AddToBucket(chunk,tempKey, (i8*) tempChar);   }while(0)

struct bucket
{
	i32 id;
	bool32 deleted;
	s32 *key;
	bool32 filled;
	i8 *value;
	struct bucket *next;
};


struct bucket_list
{
	/*NOTE: leave this for now until the array is fixed*/
	int keys[256];
	i32 total;
	i32 count;
	struct bucket *buckets;
	HASHFUNCTION hashFunction;
	bool32 deleted;
};

i32 DefaultHashFunction(s32 *key, i8 *value)
{
	i32 keyLen = Strlen(key);
	i32 index = -1;

	for (int i=0;i<keyLen;i++)
	{
		index += key[i];
	}

	return index;
}

void InitBucket(struct bucket_list *buckets,int numberOfBuckets,HASHFUNCTION hashFunction)
{
	Assert(numberOfBuckets > 5);
	buckets->total = numberOfBuckets;
	buckets->count = 0;
	buckets->buckets = NULL;
	buckets->hashFunction = hashFunction;
	//buckets->keys = CreateArray(int);
}



void AddToBucket(struct bucket_list *buckets, s32 *key,i8* value)
{
	i32 index = 0;

	Assert(buckets->total > 5);

	if (buckets->hashFunction == NULL)
	{
		buckets->hashFunction = DefaultHashFunction;
	}

	index = (buckets->hashFunction)(key,NULL);

	index = index / buckets->total;

	while (index  >=  buckets->total)
	{
		index = index / buckets->total;
	}

	buckets->keys[buckets->count] = index;
	//AddToArray(  buckets->keys,  index);

	if (buckets->buckets == NULL)
	{
		buckets->buckets = MemoryA(struct bucket,buckets->total);
	}

	struct bucket *newBucket = buckets->buckets + index;

	if (newBucket->filled == false)
	{
		newBucket->id = buckets->count;
		newBucket->deleted = false;
		newBucket->filled = true;
		newBucket->key = key;
		newBucket->value= (i8*) value;
		newBucket->next = NULL;
	} else {
		
		struct bucket *nextNewBucket = Memory(struct bucket);
		nextNewBucket->id = buckets->count;
		nextNewBucket->filled = true;
		nextNewBucket->key =  key;
		nextNewBucket->value=(i8*)   value;
		//nextNewBucket->next = NULL;
		
		if (newBucket->next)
		{
			nextNewBucket->next = newBucket->next;
			newBucket->next = nextNewBucket;
			/*
			newBucket->next->next =  Memory(bucket);
			newBucket->next->next->id = buckets->count;
			newBucket->next->next->filled = true;
			newBucket->next->next->key = S32(key);
			newBucket->next->next->value= (i8*)value;
			newBucket->next->next->deleted =  false;
			newBucket->next->next->next = NULL;

			newBucket->next= newBucket->next->next;
			*/
		} else {
			newBucket->next = nextNewBucket;
			/*
			newBucket->next=  Memory(bucket);
			newBucket->next->id = buckets->count;
			newBucket->next->filled = true;
			newBucket->next->key = S32(key);
			newBucket->next->deleted = false;
			newBucket->next->value= (i8*)value;
			newBucket->next->next = NULL;
		//	newBucket = nextNewBucket;
		*/
		}
	}
	buckets->count++;
}


struct bucket *GetBucketFromKey(struct bucket_list *buckets,char* key)
{
	struct bucket *result = NULL;
    i32 index = 0;
    
    if (buckets->hashFunction == NULL)
    {
        buckets->hashFunction = DefaultHashFunction;
    }
    
    index = (buckets->hashFunction)(key,NULL);

	index = index / buckets->total;
	while (index  >  buckets->total)
	{
		index = index / buckets->total;
	}

	Assert(index > -1);


	struct bucket *tempBucket = (struct bucket*) (buckets->buckets + index);

	if (tempBucket)
	{
		if (tempBucket->key)
		{
			if (StrCmp(key,tempBucket->key))
			{
				result = tempBucket;
			}
		} else {
			struct bucket *currentBucket = tempBucket->next;

			while(currentBucket != NULL)
			{
				if (currentBucket->key)
				{
					if (StrCmp(key,currentBucket->key))
					{
						result = currentBucket;
						break;
					}
				}

				currentBucket = currentBucket->next;
			}
		}
	}


	return result;
}




i8 *GetFromBucket(struct bucket_list *buckets,char* key)
{
	i8  *result = NULL;
    i32 index = 0;
    if (buckets->hashFunction == NULL)
    {
        buckets->hashFunction = DefaultHashFunction;
    }       
    index = (buckets->hashFunction)(key,NULL);

	index = index / buckets->total;
	while (index  >  buckets->total)
	{
		index = index / buckets->total;
	}

	Assert(index > -1);
	struct bucket *tempBucket = (struct bucket*) (buckets->buckets + index);

	if (tempBucket)
	{
        if (buckets->count > 0)
        {
            if (StrCmp(key,tempBucket->key))
            {
                result =  tempBucket->value;
            } else {
                struct bucket *currentBucket = tempBucket->next;

                while(currentBucket !=  NULL)
                {
                    if (StrCmp(key,currentBucket->key))
                    {
                        result = currentBucket->value;
                        break;
                    }

                    currentBucket = currentBucket->next;
                }
            }
		}
	}


	return result;
}

void BucketPrintAllKeys(i8* chunk)
{
#if 0
	struct array_info *chunkInfo = (struct array_info *) chunk;
	int *allKeys = (int*) (chunk + sizeof(struct array_info));

	for (int i=0;i < chunkInfo->count;i++)
	{
		printf("%i\n", allKeys[i]);
	}
#endif
}




void FreeAllBuckets(struct bucket_list *buckets)
{
	//struct array_info *chunkInfo = (struct array_info *) buckets->keys;
//	int *allKeys = (int*) ( (i8*)buckets->keys+ sizeof(array_info));
	int i=0;
	for (i=0;i < buckets->count;i++)
	{
		int index =  buckets->keys[i];
		struct bucket *buck = buckets->buckets + index;
		if (buck->deleted == false)
		{
			if (buck->key)
			{
				Free( buck->key);
				buck->key=NULL;
			}
			if (buck->value)
			{
				Free(buck->value);
				buck->value=NULL;
			}

			if (buck->next)
			{
				struct bucket *nextBucketBlock = buck->next;

				while(nextBucketBlock)
				{
					if (nextBucketBlock)
					{
						struct bucket *tempNextBucketBlock = nextBucketBlock;
						nextBucketBlock = nextBucketBlock->next;
						
						if (tempNextBucketBlock->key)
						{
							FreeMemory( (i8*)tempNextBucketBlock->key);
							tempNextBucketBlock->key=NULL;
						}

						if (tempNextBucketBlock->value)
						{
							FreeMemory((i8*)tempNextBucketBlock->value);
							tempNextBucketBlock->value=NULL;
						}
						
						if (tempNextBucketBlock)
						{
							FreeMemory((i8*)tempNextBucketBlock);
							tempNextBucketBlock=NULL;
						}
					}
				}
			}
			buck->deleted = true;
		}
	}

	if (buckets->buckets)
	{
		FreeMemory((i8*)buckets->buckets);
		buckets->buckets=NULL;
	}
    buckets->count = 0;
	/*
	if (buckets->keys)
	{
		FreeMemory(buckets->keys);
		buckets->keys=NULL;
	}
	"*/
}


#endif
