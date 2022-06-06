
const fetchData = () => {
  return fetch(`http://192.168.1.101:8086/query?db=testdb&q=SELECT "time","cah","cur","cwh","pow","rwo","vbus" FROM lion where time > now()-8h`)
//  return fetch(`http://192.168.1.101:8086/query?db=testdb&q=SELECT "time","cah","cur","cwh","pow","rwo","vbus" FROM lion where time >= '2022-03-16T03:00:00Z' AND time <= '2022-03-16T08:00:00Z'`)
//  return fetch(`http://localhost:8086/query?db=testdb&q=SELECT "vbus" FROM "lion"`)
    .then( response => {
      if (response.status !== 200) {
        console.log(response);
      }
      return response;
    })
    .then( response => response.json() )
    .then( parsedResponse => {
      const data = [];
      parsedResponse.results[0].series[0].values.map( (elem, i) => {
        let newArr = [];
        newArr.push(new Date(Date.parse(elem[0])));
        newArr.push(elem[1]); // cah pojemność Ah
        newArr.push(elem[2]); // cur prąd
        newArr.push(elem[3]); // cwh pojemność Wh
        newArr.push(elem[4]); // pow moc
        newArr.push(elem[5]); // rwo rezystancja wewnetrzna
        newArr.push(elem[6]); // vbus napiecie
        data.push(newArr);
      });
      return data;

    })
    .catch( error => console.log(error) );
}

const drawGraph = () => {  let g;
  Promise.resolve(fetchData())
    .then( data => {
			const xdata = ["zmienna",2.1,3.1];
			document.getElementById("demo").innerHTML = xdata[0];
      g = new Dygraph( document.getElementById("div_g"), data,
        {
          drawPoints: false,
					rollPeriod: 1,
          showRoller: true,
          title: 'Wykres',
					//  titleHeight: 32,
          ylabel: 'Data',
          xlabel: 'Time',
					//  strokeWidth: 1.0,
					//  labels: ['Date', 'Price'],
					labelsSeparateLines: true,
					//pointSize : 5,
					legend : 'always'
					
        });
        
      
    });

		window.setInterval( () => {
			console.log(Date.now());
			Promise.resolve(fetchData())
				.then( data => { g.updateOptions( { 'file': data } ); });
		}, 2000);
}

$(document).ready(drawGraph());

	function odlicz(){
		//const arr = ["a","b","c"];
		//let len = data.length;
		var dzis = new Date();
		var day = dzis.getDate()+1;
		var sec = dzis.getSeconds();

		document.getElementById("demo").innerHTML = day+"/"+sec;
		//document.getElementById("demo").innerHTML = 1;
		//document.write(dat);
		setTimeout("odlicz()",1000);
		console.log("odp1");
	}
