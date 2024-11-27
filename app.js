// LED 상태 업데이트
const bulb = document.getElementById('bulb');
bulb.addEventListener('click', () => {
    if (bulb.classList.contains('bulb-off')) {
        bulb.classList.remove('bulb-off');
        bulb.classList.add('bulb-on');
    } else {
        bulb.classList.remove('bulb-on');
        bulb.classList.add('bulb-off');
    }
});

// 부저 상태 업데이트
const buzzer = document.getElementById('buzzer');
buzzer.addEventListener('click', () => {
    if (buzzer.classList.contains('buzzer-off')) {
        buzzer.classList.remove('buzzer-off');
        buzzer.classList.add('buzzer-on');
    } else {
        buzzer.classList.remove('buzzer-on');
        buzzer.classList.add('buzzer-off');
    }
});
