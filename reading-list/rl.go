package main

import (
  "fmt"
  "flag"
  "os"
  "net/http"
  "io/ioutil"
  "encoding/json"
  "strings"
)

type Book struct {
  Title, Authors string
}

func NewBook(title, authors string) *Book {
  return &Book{title, authors}
}

func BookFromIsbn(isbn string) *Book {
  //fetch the books data from the google book API
  response, _ := http.Get("https://www.googleapis.com/books/v1/volumes?q=isbn:" + isbn)
  body, _ := ioutil.ReadAll(response.Body)

  //parse the response into a Book
  var data interface{}
  json.Unmarshal(body, &data)
  mappedData := data.(map[string]interface{})
  items := mappedData["items"].([]interface{})
  item := items[0].(map[string]interface{})
  volumeInfo := item["volumeInfo"].(map[string]interface{})
  title := volumeInfo["title"].(string)
  authors := volumeInfo["authors"].([]interface{})

  authorSlice := make([]string, 0)
  for _, element := range authors {
    authorSlice = append(authorSlice, element.(string))
  }

  return NewBook(title, strings.Join(authorSlice, ", "))
}

func main() {
  isbn := flag.String("isbn", "none", "The ISBN of the book we're looking for")
  flag.Parse()

  //check that we've been provided with an isbn
  if ("none" == *isbn) {
    fmt.Println("Requires an ISBN with the -isbn flag")
    os.Exit(2)
  }

  book := BookFromIsbn(*isbn)
  fmt.Printf("%s by %s\n", book.Title, book.Authors)
}
