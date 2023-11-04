"use strict";

window.scrollBehavior = "smooth";
document.scrollBehavior = "smooth";

const home_link = document.querySelector(".home");
const about_link = document.querySelectorAll(".about");
const contact_link = document.querySelector(".contact");

home_link.addEventListener("click", () => {
  const home = document.getElementById("home");
  home.scrollIntoView({
    behavior: "smooth",
    block: "start",
    inline: "nearest",
  });
});

about_link.forEach((ele) =>
  ele.addEventListener("click", () => {
    const about = document.getElementById("about");
    about.scrollIntoView({
      behavior: "smooth",
      block: "start",
      inline: "nearest",
    });
  })
);

contact_link.addEventListener("click", () => {
  const contact = document.getElementById("contact");
  contact.scrollIntoView({
    behavior: "smooth",
    block: "start",
    inline: "nearest",
  });
});
