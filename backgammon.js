jQuery.prototype.svg = function(tag,attr) {
  let x = $(document.createElementNS('http://www.w3.org/2000/svg',tag))
    .appendTo(this);
  if (attr) x.attr(attr);
  return x;
}

function last(xs) { return xs[xs.length-1]; }

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
  let moved = 0, nmoves = 0;

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

  const g_point_nums = g_text.svg('g',{'class':'point_nums'});
  for (const x of [[1,0,0],[7,-140,0],[13,-240,-230],[19,-100,-230]]) {
    const g = g_point_nums.svg('g');
    for (let i=0; i<6; ++i)
      g.svg('text',{
        x:260+i*(x[2]?20:-20), y:237, transform:'translate('+x[1]+','+x[2]+')'
      }).text(i+x[0]);
  }

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
      set_board();
    }
  });

  // Buttons ========================================================
  const g_buttons = board.svg('g',{'class': 'noselect'});
  function draw_button(text,attr={}) {
    const g = g_buttons.svg('g',{
      'class': 'button',
      ...attr
    });
    const r = g.svg('rect');
    const tbb = g.svg('text').text(text)[0].getBBox();
    r.attr({
      width: 1.15*tbb.width, height: 1.05*tbb.height,
      x: -0.075*tbb.width, y: -0.85*tbb.height, rx: 1.5
    });
    return g;
  }
  const submit_button = draw_button('Submit',{
    transform: 'translate(157,123)', visibility: 'hidden'
  }).on('click',function(){
    alert('Sumbitting not yet implemented');
  });
  const cancel_button = draw_button('Cancel',{
    transform: 'translate(240,123)', visibility: 'hidden'
  }).on('click',set_board);

  // set board ======================================================
  const pos_bin = atob(init_pos_str);
  let pos, g_bearoff;

  function set_board() {
    g_checkers.empty();
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

    g_bearoff = g_text.svg('g');
    g_bearoff.svg('text',{'x':275,'y':50,'class':'bearoff'})
      .text(pos[51]);
    g_bearoff.svg('text',{'x':275,'y':190,'class':'bearoff player'})
      .text(pos[50]);

    moved = 0;
    nmoves = 2; // TODO: calculate

    submit_button.attr('visibility','hidden');
    cancel_button.attr('visibility','hidden');
  }
  set_board();

  // Checker action =================================================
  function top_checker_player(a) {
    return last(g_checkers.find('.player[point='+a+']'));
  }
  function top_checker_not_player(a) {
    return last(g_checkers.find(':not(.player)[point='+a+']'));
  }
  function checker_click() {
    if (moved >= nmoves) return;
    const a = this.getAttribute('point');
    const b1 = a-dice[0], b2 = 49-b1;
    const nopp = pos[b2];
    if (b1<1) {
      let outfield = 0;
      for (let i=6; i<24; ++i) outfield += pos[i];
      if (outfield>0) alert('Can\'t bear off yet. '
        + outfield.toString() + ' checkers in the outfield.');
      else {
        top_checker_player(a).remove();
        --pos[a-1];
        g_bearoff.find('.bearoff.player').text(++pos[50]);
        ++moved;
      }
    } else if (nopp<2) {
      let c = top_checker_player(a);
      console.log(c);
      const cl = c.getAttribute('class').replace(/\bchecker /g,'');
      c.remove();
      --pos[a-1];
      draw_checker(cl,b1,pos[b1-1]++).on('click',checker_click);
      ++moved;
      if (nopp==1) {
        top_checker_not_player().remove();
        --pos[b2];
      }
    } else {
      alert('Illegal move '+a.toString()+' to '+b1.toString());
    }
    console.log('moved: '+moved);
    if (moved) {
      cancel_button.attr('visibility','visibile');
      if (moved==nmoves)
        submit_button.attr('visibility','visibile');
    }
  }

});
