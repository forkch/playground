package ch.fork.rxretrofit;

import retrofit.RestAdapter;
import retrofit.http.GET;
import retrofit.http.Path;
import rx.Observable;
import rx.functions.Func1;

import java.util.List;
import java.util.concurrent.TimeUnit;

/**
 * Created by fork on 15.07.14.
 */
public class RxRetrofitTest {

    interface GitHub {

        @GET("/users/{username}/repos")
        Observable<List<Repository>> getUserRepositories(@Path("username") String username);

    }

    public static void main(String[] args) {
        RestAdapter adapter = new RestAdapter.Builder().setEndpoint("https://api.github.com").build();

        GitHub gitHub = adapter.create(GitHub.class);


        Observable<List<Repository>> repositories = gitHub.getUserRepositories("forkch");

        repositories.flatMap(repositoryList -> Observable.from(repositoryList)).delay(10, TimeUnit.SECONDS).forEach(r -> System.out.println(r.getName()));

    }

}
