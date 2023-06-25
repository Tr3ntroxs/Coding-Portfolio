var hamburger = document.querySelector(".hamb");
var navList = document.querySelector(".submenus");
var links = document.querySelector(".submenus li");

hamburger.addEventListener("click", function(){
    this.classList.toggle("click");
    navList.classList.toggle("open");
});