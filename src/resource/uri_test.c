#include <stdlib.h>
#include "uri.h"
#include "../assertion.h"

void testParseInvalidUri() {
    char input[] = "dragon.mtl";
    struct URI uri;
    int result = parseURI(&uri, input);
    assertIntegersEqual(result, 1);
}

void testParseFileScheme() {
    char input[] = "file:///path/to/file.ext";
    struct URI uri;
    int result = parseURI(&uri, input);
    assertIntegersEqual(result, 0);

    assertStringsEqual(uri.scheme, "file");
    assertStringsEqual(uri.authority.host, "");
    assertStringsEqual(uri.path, "/path/to/file.ext");
}

void testParseFileScheme2() {
    char input[] = "file:///config.json";
    struct URI uri;
    int result = parseURI(&uri, input);
    assertIntegersEqual(result, 0);

    assertStringsEqual(uri.scheme, "file");
    assertStringsEqual(uri.authority.host, "");
    assertStringsEqual(uri.path, "/config.json");
}

void testParseFileScheme3() {
    char input[] = "file:/config.json";
    struct URI uri;
    int result = parseURI(&uri, input);
    assertIntegersEqual(result, 0);

    assertStringsEqual(uri.scheme, "file");
    assertPointersEqual(uri.authority.host, NULL);
    assertStringsEqual(uri.path, "/config.json");
}

void testParseURI() {
    char input[] = "https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top";
    struct URI uri;
    int result = parseURI(&uri, input);
    assertIntegersEqual(result, 0);

    assertStringsEqual(uri.scheme, "https");
    assertStringsEqual(uri.authority.host, "john.doe@www.example.com:123"); // TODO
    assertStringsEqual(uri.path, "/forum/questions/");
    assertStringsEqual(uri.query, "tag=networking&order=newest");
    assertStringsEqual(uri.fragment, "top");
}

void testUriToStr() {
    char input[] = "https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top";
    char *expected = input;
    struct URI uri;
    int result = parseURI(&uri, input);
    assertIntegersEqual(result, 0);

    char *actual = uriToStr(&uri);
    assertStringsEqual(actual, expected);
}

void testUriSwapExt() {
    char input[] = "https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top";
    char *expected = "https://john.doe@www.example.com:123/forum/questions/.txt?tag=networking&order=newest#top";
    struct URI uri;
    int result = parseURI(&uri, input);

    assertIntegersEqual(result, 0);

    char *actual = uriSwapExt(&uri, "txt");
    assertStringsEqual(actual, expected);
    free(actual);
}

void testUriSwapFile() {
    char input[] = "https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top";
    char *expected = "https://john.doe@www.example.com:123/forum/questions/dragon.mtl?tag=networking&order=newest#top";
    struct URI uri;
    int result = parseURI(&uri, input);

    assertIntegersEqual(result, 0);

    char *actual = uriSwapFile(&uri, "dragon.mtl");
    assertStringsEqual(actual, expected);
    free(actual);
}

void testUriSwapFile2() {
    char input[] = "file:/data/dragon/dragon.obj";
    char *expected = "file:/data/dragon/dragon.mtl";
    struct URI uri;
    int result = parseURI(&uri, input);

    assertIntegersEqual(result, 0);

    char *actual = uriSwapFile(&uri, "dragon.mtl");
    assertStringsEqual(actual, expected);
    free(actual);
}

void uriTest() {
    testParseInvalidUri();
    testParseFileScheme();
    testParseFileScheme2();
    testParseFileScheme3();
    testParseURI();
    testUriToStr();
    testUriSwapExt();
    testUriSwapFile();
    testUriSwapFile2();
}
