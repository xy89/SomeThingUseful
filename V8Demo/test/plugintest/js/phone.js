var value = 0;
var data ;

function getValue(args) {
  return gTaskContext.getStepCount(arguments[0]);
}

function setValue(args) {
    value = args[0];
}

function getValue2(){
  return value;
}

function setData(args){
    data = arguments[0].data;
}

function getData(){
    return data;
}
