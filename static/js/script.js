'use strict';

$(document).ready(function () {
    var data = [];
    var queryCnt = 100; // 한 번에 가져올 데이터 개수
    var sensorData = {}; // 센서 데이터 저장 객체
    var res1 = []; // IR 데이터
    var res2 = []; // TILT 데이터
    var res3 = []; // Light 데이터

    
    var options = {
        colors: ["#30323c"],
        series: {
            shadowSize: 0,
            lines: {
                show: true,
                fill: true,
                fillColor: {
                    colors: [{ opacity: 0.5 }, { opacity: 0.5 }]
                }
            }
        },
        yaxis: {
            min: 0,
            max: 1,
            tickSize: 1
        },
        xaxis: {
            show: false,
            min: 0,
            max: queryCnt
        },
        points: { show: true },
        grid: {
            backgroundColor: '#fff',
            borderWidth: 1,
            borderColor: '#fff',
            hoverable: true
        }
    };

    
    var plot1 = $.plot($("#realtime1"), [res1], options);

    
    var plot2 = $.plot($("#realtime2"), [res2], options);

    
    var plot3 = $.plot($("#realtime3"), [res3], options);

    
    function getData() {
        $.ajax({
            url: "/sensor/getSensor/" + queryCnt,
            type: "GET",
            dataType: "json",
            success: (res) => {
                data = res;

                
                res1 = [];
                res2 = [];
                res3 = [];

                for (var i = 0; i < data.length; ++i) {
                    var pairs = data[i]['value'];
                    var lis = pairs.split(",");
                    lis.forEach(pair => {
                        const [key, value] = pair.split(':');
                        sensorData[key.trim()] = parseFloat(value);
                    });

                    
                    res1.push([i, sensorData['IR']]);
                    res2.push([i, sensorData['TILT']]);
                    res3.push([i, sensorData['Light']]);
                }

                // 차트 업데이트 함수 호출 (데이터를 받은 후에만 업데이트)
                updateCharts();
            },
            error: (error) => {
                console.log("데이터를 가져오는 중 오류 발생:", error);
                setTimeout(getData, updateInterval);
            }
        });
    }

    // 차트를 업데이트하는 함수
    function updateCharts() {
        // 각 차트에 데이터 설정 및 갱신
        plot1.setData([res1]);
        plot2.setData([res2]);
        plot3.setData([res3]);
        console.log(sensorData);
        plot1.draw();
        plot2.draw();
        plot3.draw();

        setTimeout(getData, updateInterval); // 일정 시간 후 다시 데이터를 가져옴
    }

    // 업데이트 간격 설정 및 시작
    var updateInterval = 1000; // 1초마다 업데이트

    // 첫 번째 호출로 데이터를 가져오기 시작
    getData();
});