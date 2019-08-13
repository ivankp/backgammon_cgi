jQuery.prototype.svg = function(tag,attr) {
  let x = $(document.createElementNS('http://www.w3.org/2000/svg',tag))
    .appendTo(this);
  if (attr) x.attr(attr);
  return x;
}

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
    'class': 'board field',
    'width': 260, 'height': 220,
    'x': 10, 'y': 10
  });
  g0.svg('path',{
    'class': 'board box',
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

  const g_text = board.svg('g',{'class':'noselect'});

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
      r: 9.5,
      point: p
    });
  }

  function checker_click() {
    const a = this.getAttribute('point');
    const b1 = a-dice[0], b2 = 49-b1;
    const nopp = pos[b2];
    if (b1<1) {
      alert('Bearing off not yet implemented');
    } else if (nopp<2) {
      const cs = g_checkers.find('.player[point='+a+']');
      let c = cs[cs.length-1];
      const cl = c.getAttribute('class').replace(/\bchecker /g,'');
      c.remove();
      --pos[a-1];
      draw_checker(cl,b1,pos[b1-1]++).on('click',checker_click);
      ++moved;
      if (nopp==1) {
        const cs = g_checkers.find(':not(.player)[point='+b1+']');
        let c = cs[cs.length-1];
        c.remove();
        --pos[b2];
      }
    } else {
      alert('Illegal move '+a.toString()+' to '+b1.toString());
    }
    console.log('moved: '+moved);
  }

  // set board ======================================================
  const pos_bin = atob(init_pos_str);
  let pos;

  function set_board() {
    pos = [0];
    loop_i: for (let i=0; i<pos_bin.length; ++i) {
      const c = pos_bin.charCodeAt(i);
      let mask = 1;
      for (let j=0; j<8; ++j) {
        if (c & mask) ++pos[pos.length-1];
        else if (pos.length==50) break loop_i;
        else pos.push(0);
        mask = mask << 1;
      }
    }

    // let p; // swap
    // for (let i=0; i<25; ++i) {
    //   p = pos[i]
    //   pos[i] = pos[i+25];
    //   pos[i+25] = p;
    // }

    for (let i=0; i<24; ++i) {
      for (let j=0; j<pos[i]; ++j)
        draw_checker((setup[3]==='w'?'white':'black')+' player',i+1,j)
        .on('click',checker_click);
      for (let j=0; j<pos[i+25]; ++j)
        draw_checker((setup[3]==='w'?'black':'white'),24-i,j);
    }

    // compute number borne off
    pos.push(15);
    pos.push(15);
    for (let i=0; i<2; ++i)
      for (let j=0; j<25; ++j)
        pos[50+i] -= pos[j+(i?25:0)];

    console.log(pos);
    console.log(pos[50]);
    console.log(pos[51]);

    const g_bearoff = g_text.svg('g');
    g_bearoff.svg('text',{'x':275,'y':50,'class':'bearoff'})
      .text(pos[51]);
    g_bearoff.svg('text',{'x':275,'y':190,'class':'bearoff player'})
      .text(pos[50]);

    moved = 0;
  }
  set_board();

  // Dice ===========================================================
  const dice = [ parseInt(setup[0]), parseInt(setup[1]) ];
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
  }
  function draw_dice() {
    draw_die(dice[0],192);
    draw_die(dice[1],212);
  }
  draw_dice();
  g_dice.on('click',function() {
    dice.reverse();
    g_dice.empty();
    draw_dice();
    if (moved) {
      g_checkers.empty();
      set_board();
    }
  });
});
