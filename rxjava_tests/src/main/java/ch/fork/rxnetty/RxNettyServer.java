package ch.fork.rxnetty;

import io.netty.buffer.ByteBuf;
import io.netty.handler.codec.http.HttpResponseStatus;
import io.reactivex.netty.RxNetty;
import io.reactivex.netty.protocol.http.server.HttpServer;

/**
 * Created by fork on 15.07.14.
 */
public class RxNettyServer {
    public static void main(String... args) throws InterruptedException {
        HttpServer<ByteBuf, ByteBuf> server = RxNetty.createHttpServer(8080, (request, response) -> {
            System.out.println("Server => Request: " + request.getPath());
            try {
                if ("/error".equals(request.getPath())) {
                    throw new RuntimeException("forced error");
                }
                response.setStatus(HttpResponseStatus.OK);
                response.writeString("Path Requested =>: " + request.getPath() + '\n');
                return response.close();
            } catch (Throwable e) {
                System.err.println("Server => Error [" + request.getPath() + "] => " + e);
                response.setStatus(HttpResponseStatus.BAD_REQUEST);
                response.writeString("Error 500: Bad Request\n");
                return response.close();
            }
        });

        server.startAndWait();

    }

}
