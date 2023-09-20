import { ToDoItem } from "./ToDoItem"

export function ToDoList( {todos, toggleTodo, deleteTodo} ) {
    return <ul className="list">
    {todos.length === 0 && "No ToDos"}
    {todos.map(todo => {
        return <ToDoItem {...todo} key={todo.id} toggleTodo={toggleTodo} deleteTodo={deleteTodo}/>
    })}
  </ul>
}