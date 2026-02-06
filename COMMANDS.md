# Serial Commands Reference

## Mode Control

```
SET_MODE clock
SET_MODE hourglass
```

## Clock Mode

```
SET_TIME 14 30
```
(Format: SET_TIME [hours 0-23] [minutes 0-59])

## Hourglass Mode

```
RESET_HG
```

## Status & Info

```
GET_STATUS
GET_ORIENTATION
GET_DISPLAY
```

## Settings

```
SET_BRIGHTNESS 8
```
(Range: 0-15)

## Responses

```
OK MODE CLOCK
OK MODE HOURGLASS
OK TIME 14:30
OK HOURGLASS RESET
OK BRIGHTNESS SET
ERR [error message]
{...json data...}
```

## Auto-Updates (sent every 500ms)

```
STATUS:{"mode":"clock","hours":14,"minutes":30,"orientation":0}
```
