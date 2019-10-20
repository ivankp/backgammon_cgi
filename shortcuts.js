document.addEventListener("keypress",event => {
  let x = event.which || event.keyCode;
  switch (x) {
    case 103: // g
      window.location.replace('.'); break;
    case 110: // n
      window.location.replace('?g'); break;
    // default:
    //   console.log(x);
  }
});
