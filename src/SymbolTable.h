/*
 * SymbolTable.h
 *
 *  Created on: 2 Jun 2014
 *      Author: na4113
 */

#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_


	const int IN = 23;

	struct SymTable {
		char *labels;
		uint32_t *symbols;
		struct SymTable *first;
		struct SymTable *next;
	};




    struct SymTable ST_new(void);

    void ST_free(struct SymTable oSymTable);

    int ST_getLength(struct SymTable oSymTable);

    int ST_put(struct SymTable oSymTable, const char *pcKey, const void *pvValue);

    int ST_remove(struct SymTable oSymTable, const char *pcKey);

    int ST_contains(struct SymTable oSymTable, const char *pcKey);

    void *ST_get(struct SymTable oSymTable, const char *pcKey);

    void ST_map(struct SymTable oSymTable,
       void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
       const void *pvExtra);



#endif /* SYMBOLTABLE_H_ */
