import asyncio

DEFAULT_PATH = '/http2server-stream'


# TODO: debug the bug
async def _test_body_stream(http2_client, service_client, dynamic_config):
    part = 'part'
    count = 100
    r = await http2_client.get(
        DEFAULT_PATH,
        params={'type': 'eq', 'body_part': part, 'count': count},
    )
    assert 200 == r.status_code
    assert part * count == r.text


async def _stream_request(client, req_per_client):
    for _ in range(req_per_client):
        data = 'abcdefgh' * 2**7  # size is 1024
        r = await client.get(DEFAULT_PATH, params={'type': 'ne'}, data=data)
        assert 200 == r.status_code
        assert data == r.text


async def test_body_stream_small_pieces(
    http2_client,
    service_client,
    dynamic_config,
):
    _stream_request(http2_client, 1)


async def test_body_stream_concurrent(
    http2_client,
    service_client,
    dynamic_config,
):
    clients_count = 2
    req_per_client = 10
    tasks = [_stream_request(http2_client, req_per_client) for _ in range(clients_count)]
    await asyncio.gather(*tasks)
