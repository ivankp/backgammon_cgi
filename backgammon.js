function load_game(gid) {
  return $.getJSON('cgi/load_game.py',{'id':gid},function(data){
    console.log(data);
  });
}

jQuery.prototype.svg = function(tag,attr) {
  let x = $(document.createElementNS('http://www.w3.org/2000/svg',tag))
    .appendTo(this);
  if (attr) x.attr(attr);
  return x;
}

function board_setup(game) {
  document.title = 'G'+game.id+': '+game.player1+' vs '+game.player2;

  const aspect = 7./6.;
  const board = $('#board').svg('svg',{ 'viewBox': '0 0 280 240' });
  const left_div = $('#left'), under_div = $('#under'), info_div = $('#info');
  let info_under = false;
  function resize() {
    let w0 = $(window).width(), h0 = $(window).height()-32-5, w=w0, h=h0;
    if (w0/h0 < aspect) h = w / aspect;
    else                w = h * aspect;
    board.attr({ 'width': w, 'height': h });

    let dw = w0-w;
    let lw = 0;
    if (dw >= 300) lw = dw/2;
    else if (dw >= 150) lw = dw;
    left_div.css({'width': lw+'px'});
    if (dw >= 150) {
      if (info_under) {
        info_div.appendTo(left_div);
        info_under = false;
      }
    } else {
      if (!info_under) {
        info_div.appendTo(under_div);
        info_under = true;
      }
    }
  }
  resize();
  $(window).resize(resize);

  // Draw board =====================================================
  const defs = board.svg('defs');
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
  const g_6 = defs.svg('symbol',{id:'quarter'}).svg('g');
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
  g0.svg('use',{href:'#quarter',transform:'translate(150,0)'});
  g0.svg('use',{href:'#quarter',transform:'translate(10,0)'});
  g0.svg('use',{href:'#quarter',transform:'rotate(180,135,10) translate(0,-220)'});
  g0.svg('use',{href:'#quarter',transform:'rotate(180,135,10) translate(140,-220)'});

  const g_point_nums = g_text.svg('g',{'class':'point_nums'});
  for (const x of [[1,0,0],[7,-140,0],[13,-240,-230],[19,-100,-230]]) {
    const g = g_point_nums.svg('g');
    for (let i=0; i<6; ++i)
      g.svg('text',{
        x:260+i*(x[2]?20:-20), y:237, transform:'translate('+x[1]+','+x[2]+')'
      }).text(i+x[0]);
  }

  // Common =========================================================
  let nmoves, moves, borneoff;

  const colors = ['white','black'];
  // if (game.state[2]=='b')
    [colors[0],colors[1]] = [colors[1],colors[0]];

  // Players ========================================================
  const g_players = g_text.svg('g',{'class':'players'});
  g_players.svg('text',{x:125,y:114}).text(game.player2);
  g_players.svg('text',{x:125,y:132}).text(game.player1);

  for (let i=0; i<2; ++i)
    $('#'+colors[i]+'_player').text(game['player'+(i+1)]);

  // Pip count ======================================================
  const g_pip = g_text.svg('g',{'class':'pip'});
  g_pip.svg('text',{x:140,y:132});
  g_pip.svg('text',{x:140,y:114});

  function pip_count() {
    const points = g_checkers.children();
    const pip = [0,0];
    for (let p of points) {
      p = $(p);
      const pi = p.index();
      for (let i=0; i<2; ++i)
        pip[i] += ( (i==0 || pi==24) ? pi+1 : 24-pi )
                * p.find('.'+colors[i]).length;
    }
    for (let i=0; i<2; ++i)
      g_pip.children()[i].textContent = pip[i];
  }

  // Checkers =======================================================
  const g_checkers = board.svg('g',{'class':'checkers'});
  function draw_checker(c,p) {
    const gs = g_checkers.children();
    const g = $(gs[p]);
    const n = p!=24 ? g.children().length
                    : g.find('.'+colors[c]).length;
    const checker = g.svg('circle',{ 'class': colors[c], r: 9.5 }).attr(
      p < 24 ? {
        cx: (p<12 ? 260 : -200) + p*(p<12 ? -20 : 20) - (5<p && p<18 ? 20 : 0),
        cy: 20 + (p<12 ? 200-n*19 : n*19),
      } : {
        cx: 140,
        cy: (c ? 150+n*19 : 90-n*19),
      });
    if (!c) checker.addClass('player active').on('click',checker_click);
    return checker;
  }

  // Bearoff ========================================================
  const g_bearoff = g_text.svg('g');
  g_bearoff.svg('text',{'x':275,'y':190,'class':'bearoff'});
  g_bearoff.svg('text',{'x':275,'y':50,'class':'bearoff'});

  // Dice ===========================================================
  const dice = Array.from(game.dice.toString()).map(Number);
  const g_dice = board.svg('g',{'class': 'dice active'});
  const die_pips = [
    [[8,8]],
    [[5,11],[11,5]],
    [[3,13],[8,8],[13,3]],
    [[4,4],[4,12],[12,4],[12,12]],
    [[4,4],[4,12],[12,4],[12,12],[8,8]],
    [[4.5,13],[4.5,8],[4.5,3],[11.5,13],[11.5,8],[11.5,3]]
  ];
  function draw_die(x,pos) {
    const g = g_dice.svg('g',{ 'transform':'translate('+pos+',112)' });
    g.svg('rect',{ 'class': 'body', width: 16, height: 16, rx: 3 });
    for (const pips of die_pips[x-1])
      g.svg('circle',{ 'class': 'pip', cx: pips[0], cy: pips[1], r: 2 });
  }
  function draw_dice() {
    draw_die(dice[0],192);
    draw_die(dice[1],212);
  }
  draw_dice();
  g_dice.on('click',function() {
    if (dice[0]!=dice[1]) {
      dice.reverse();
      g_dice.empty();
      draw_dice();
    }
    if (moves.length) {
      set_board();
    }
  });

  // Buttons ========================================================
  const g_buttons = board.svg('g',{'class': 'noselect'});
  function draw_button(text,attr) {
    const g = g_buttons.svg('g',{'class': 'button active'});
    if (attr) g.attr(attr);
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
    $.post('cgi/move',
      dice.join('')
      + moves.reduce((a,x) => a+String.fromCharCode(x+64),'')
      + game.id
    ).done(function(data){ alert(JSON.stringify(data)); });
  });
  const cancel_button = draw_button('Reset',{
    transform: 'translate(240,123)', visibility: 'hidden'
  }).on('click',set_board);

  // set board ======================================================
  const pos_bin = atob(game.pos_current);

  function set_board() {
    g_checkers.empty();
    g_checkers.svg('g');
    borneoff = [15,15];
    let n = 0, p = 0;
    loop_i: for (let i=0; i<pos_bin.length; ++i) {
      const c = pos_bin.charCodeAt(i);
      let mask = 1;
      for (let j=0; j<8; ++j) {
        if (c & mask) {
          const checker = draw_checker(p,n);
          --borneoff[p];
        } else if (n==24) {
          if (p) break loop_i;
          else { p = 1-p; n = 47-n; }
        } else {
          if (!p) {
            g_checkers.svg('g');
            ++n;
          } else {
            if (!n) n = 24;
            else --n;
          }
        }
        mask = mask << 1;
      }
    }
    for (let i=0; i<2; ++i) // show number borne off checkers
      g_bearoff.children()[i].textContent = borneoff[i];

    moves = [ ];
    nmoves = dice[0]!=dice[1] ? 2 : 4;

    submit_button.attr('visibility','hidden');
    cancel_button.attr('visibility','hidden');

    pip_count();
  }
  set_board();

  // Checker action =================================================
  function pop_checker(p) {
    p.find('.player').last().remove();
    moves.push(p.index()+1);
    pip_count();
  }
  function checker_click() {
    if (moves.length >= nmoves) return;
    const points = g_checkers.children();
    const pa = $(this).parent();
    const  a = pa.index();
    if (a!=24 && points.last().find('.player').length > 0) {
      alert('Must come off the bar first');
      return;
    }
    const  b = a - dice[moves.length<2 ? moves.length : 0];
    const pb = b<0 ? null : $(points[b]);
    const nopp = b<0 ? 0 : pb.find(':not(.player)').length;
    if (b<0) { // Bearoff
      let outfield = 0;
      for (let i=6; i<25; ++i)
        outfield += $(points[i]).find('.player').length;
      if (outfield>0) alert('Can\'t bear off yet. '
        + outfield + ' checkers in the outfield.');
      else {
        pop_checker(pa);
        g_bearoff.children()[0].textContent = ++borneoff[0];
      }
    } else if (nopp<2) { // In-board move
      if (nopp) {
        pb.empty();
        draw_checker(1,24);
      }
      draw_checker(0,b);
      pop_checker(pa);
    } else {
      alert('Illegal move '+(a+1)+' to '+(b+1));
    }
    if (moves.length) {
      cancel_button.attr('visibility','visibile');
      if (moves.length==nmoves)
        submit_button.attr('visibility','visibile');
    }
  }

};
