function set_red()
{
  this.back_color = 0xffff0000;
}

function set_green()
{
  this.back_color = 0xff00ff00;
}

function ev_msg(msg)
{
  switch(get_id(id))
  {
    case 1210:
	  set_green();
	  break;
	case 1211:
	  set_red();
	  break;
  }
}