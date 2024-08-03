/*
stock bool:isequal(const str1[], const str2[], bool:ignorecase = false) {
stock bool:isempty(const string[]) {
stock erase(string[])
stock bool:ispassword(const password[])
*/

stock bool:isequal(const str1[], const str2[], bool:ignorecase = false) {
    new
        c1 = (str1[0] > 255) ? str1{0} : str1[0],
        c2 = (str2[0] > 255) ? str2{0} : str2[0]
    ;

    if (!c1 != !c2)
        return false;

    return !strcmp(str1, str2, ignorecase);
}


stock bool:isempty(const string[]) {
	if (ispacked(string))
		return string{0} == '\0';
	else
		return string[0] == '\0';
}

stock erase(string[])
{
    string[0] = '\0';
    return 1;
}

stock bool:isnumber(const string[])
{
    new len = strlen(string);
    for (new i; i < len; i++)
    {
        if (string[i] < '0' || string[i] > '9') return false;
    }
    return true;
}