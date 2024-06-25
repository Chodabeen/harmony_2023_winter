#include <stdio.h>
#include "parson.h" 

int main(int argc, char * argv[]) {	
    JSON_Value *rootValue;	
    JSON_Object *rootObject; 	
    rootValue = json_parse_file_with_comments("fuzzing/inputs/test1");
    if (rootValue == NULL) {
        printf("null\n");
    }    	
    rootObject = json_value_get_object(rootValue);  

    // printf("first: %s\n", json_object_get_string(rootObject, "first"));
    // printf("last: %s\n", json_object_get_string(rootObject, "last"));

    // printf("interests: ");
    // JSON_Array * array = json_object_get_array(rootObject, "interests");
    // for (int i = 0; i < json_array_get_count(array); i++) {		
    //     printf("%s ", json_array_get_string(array, i));	
    // }

    // printf("\n");

    // printf("favorites: \n");
    // JSON_Object * fav = json_object_get_object(rootObject, "favorites");
    // printf("color: %s\n", json_object_get_string(fav, "color"));
    // printf("sport: %s\n", json_object_get_string(fav, "sport"));
    

    json_value_free(rootValue);
    return 0;
}