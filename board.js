jQuery.prototype.svg = function(tag,attr) {
  let x = $(document.createElementNS('http://www.w3.org/2000/svg',tag))
    .appendTo(this);
  if (attr) x.attr(attr);
  return x;
}

// function split(str,marks) {
//   let sub = [];
//   if (!str) return sub;
//   let pos = [];
//   for (let i=0; i<marks.length; ++i)
//     pos.push(str.indexOf(marks[i], i ? pos[i-1] : 0));
//   for (let i=1; i<pos.length; ++i)
//     sub.push(str.substring(pos[i-1]+marks[i-1].length,pos[i]));
//   return sub;
// }

$(() => {
  const aspect = 7./6.;
  const board = $('#board').svg('svg',{ 'viewBox': '0 0 280 240' });
  function resize() {
    let w = $(window).width(), h = $(window).height();
    if (w/h < aspect) h = w / aspect;
    else              w = h * aspect;
    board.attr({ 'width': w, 'height': h });
  }
  resize();
  $(window).resize(resize);

  // variables ======================================================
  let moved = 0;

  // Draw board =====================================================
  const g0 = board.svg('g');
  g0.svg('rect',{
    'class': 'bg_field',
    'width': 260, 'height': 220,
    'x': 10, 'y': 10
  });
  g0.svg('path',{
    'class': 'bg_box',
    'd': 'M 0,0 0,240 280,240 280,0 0,0 z '+
         'm 10,10 120,0 0,220 -120,0 0,-220 z '+
         'm 140,0 120,0 0,220 -120,0 0,-220 z'
  });

  const g_fold = g0.svg('g');
  g_fold.svg('path',{'d':'m 139.5,2 0,235','stroke':'#774b1f'});
  g_fold.svg('path',{'d':'m 140.5,2 0,235','stroke':'#b1875a'});
  g_fold.children().css({
    'fill': 'none',
    'stroke-width': 1,
    'stroke-linecap': 'butt',
    'stroke-linejoin': 'miter',
    'stroke-opacity': 1
  });
  const g_hinge = g_fold.svg('g',{'transform':'translate(140,30)'});
  g_hinge.svg('rect',{'height':10,'width':6,'x':-3,'y':0,'fill':'#5d3d21'});
  g_hinge.svg('rect',{'height':10,'width':4,'x':-2,'y':0,'fill':'#C8BF96'});
  g_hinge.svg('rect',{'height': 1,'width':4,'x':-2,'y':5,'fill':'#774b1f'});
  g_hinge.svg('rect',{'height': 1,'width':4,'x':-2,'y':4,'fill':'#b1875a'});
  g_hinge.clone().attr('transform','translate(140,200)').appendTo(g_fold);

  // Draw points ====================================================
  const g_6 = g0.svg('g',{'transform':'translate(150,0)'});
  for (let i=0; i<3; ++i) {
    let g_trg1 = g_6.svg('path',{
      'class': 'point black',
      'transform': 'translate('+(i*40)+',10)',
      'd': 'M 0,0 10,95 20,0 z',
      'stroke': '#000',
      'stroke-width': 0.3
    });
    let g_trg2 = g_trg1.clone().attr({
      'class': 'point white',
      'transform': 'translate('+(20+i*40)+',10)'
    }).appendTo(g_6);
  }
  g_6.clone().attr('transform','translate(10,0)')
    .appendTo(g0);
  g_6.clone().attr('transform','rotate(180,135,10) translate(0,-220)')
    .appendTo(g0);
  g_6.clone().attr('transform','rotate(180,135,10) translate(140,-220)')
    .appendTo(g0);

  // Checkers =======================================================
  const g_checkers = board.svg('g');
  function draw_checker(cl,p,n) {
    return g_checkers.svg('circle',{
      'class': 'checker '+cl,
      cx: (p<13 ? 280 : -220) + p*(p<13 ? -20 : 20) - (p>6 && p<19 ? 20 : 0),
      cy: 20 + (p<13 ? 200-n*19 : n*19),
      r: 9.5
    });
  }

  // set board ======================================================
  const pos_bin = atob(init_pos_str);
  let pos = [0];

  function set_board() {
    for (let i=0; i<pos_bin.length; ++i) {
      const c = pos_bin.charCodeAt(i);
      let mask = 1;
      for (let j=0; j<8; ++j) {
        if (c & mask) ++pos[pos.length-1];
        else pos.push(0);
        mask = mask << 1;
      }
    }

    let checker = null;
    for (let i=0; i<24; ++i) {
      for (let j=0; j<pos[i]; ++j)
        checker = draw_checker('white',i+1,j);
      if (checker) {
        checker.addClass('top');
        checker = null;
      }
      for (let j=0; j<pos[i+25]; ++j)
        checker = draw_checker('black',24-i,j);
      if (checker) checker.addClass('top');
    }
  }
  set_board();

  // Dice ===========================================================
  const g_dice = board.svg('g');
  const die_pips = [
    [[8,8]],
    [[5,11],[11,5]],
    [[3,13],[8,8],[13,3]],
    [[4,4],[4,12],[12,4],[12,12]],
    [[4,4],[4,12],[12,4],[12,12],[8,8]],
    [[4.5,13],[4.5,8],[4.5,3],[11.5,13],[11.5,8],[11.5,3]]
  ];
  function draw_die(x,pos) {
    const g = g_dice.svg('g',{
      'class': 'die',
      'transform':'translate('+pos+',112)'
    });
    g.svg('rect',{ 'class': 'die body', width: 16, height: 16, rx: 3 });
    for (const pips of die_pips[x-1])
      g.svg('circle',{ 'class': 'die pip', cx: pips[0], cy: pips[1], r: 2 });
    return g;
  }
  function draw_dice() {
    draw_die(dice[0],192);
    draw_die(dice[1],212);
  }
  draw_dice();
  g_dice.on('click',function() {
    dice = [dice[1],dice[0]];
    g_dice.children().remove();
    draw_dice();
    if (moved) set_board();
  });

  // drag and drop ==================================================
  // http://www.petercollingridge.co.uk/tutorials/svg/interactive/
  // checker=null;
  // let grab_pos=null;
  // let point;
  // function startDrag(evt) {
  //   evt.preventDefault();
  //   let classes = evt.target.classList;
  //   if (classes.contains('checker') && classes.contains('top')) {
  //     checker = evt.target;
  //     checker_pos = [ checker.cx, checker.cy ];
  //     grab_pos = [ evt.clientX, evt.clientY ];
  //   }
  // }
  // function endDrag(evt) {
  //   if (checker) {
  //     // if (legalMove(checker)) {
  //     //   console.log(point);
  //     // }
  //     checker = null;
  //   }
  // }
  // function drag(evt) {
  //   if (checker) {
  //     evt.preventDefault();
  //     var CTM = evt.target.getScreenCTM();
  //     if (evt.touches) evt = evt.touches[0];
  //     // let x = (evt.clientX - grab_pos[0])/CTM.a + checker_pos[0];
  //     // let y = (evt.clientY - grab_pos[1])/CTM.d + checker_pos[1];
  //     let x = (evt.clientX - grab_pos[0])/CTM.a;
  //     let y = (evt.clientY - grab_pos[1])/CTM.d;
  //     checker.setAttributeNS(null,'transform','translate('+x+','+y+')');
  //     // point = Math.floor((x-10)/20);
  //     // if (y<100) {
  //     //   point += 20;
  //     //   if (x<-30) point += 1;
  //     //   else if (x<-10) point = 26;
  //     // } else {
  //     //   point = 4 - point;
  //     //   if (x>-10) point += 1;
  //     //   else if (x>-30) point = 27;
  //     // }
  //   }
  // }
  // board.on('mousedown',startDrag);
  // board.on('mousemove', drag);
  // board.on('mouseup', endDrag);
  // board.on('mouseleave', endDrag);
  // board.on('touchstart', startDrag);
  // board.on('touchmove', drag);
  // board.on('touchend', endDrag);
  // board.on('touchleave', endDrag);
  // board.on('touchcancel', endDrag);
});
